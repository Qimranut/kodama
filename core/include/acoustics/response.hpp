#pragma once

// Arrival
#include <acoustics/arrival.hpp>

// Timing
#include <acoustics/timing.hpp>

// Path
#include <acoustics/path.hpp>

// Spatializer
#include <audio/spatializer.hpp>

// Impulse Response
#include <audio/impulse_response.hpp>

// Response
#include <acoustics/response.hpp>

// stds
#include <vector>
#include <cmath>
#include <algorithm>
#include <array>


namespace kodama::acoustics
{
	inline std::array<float, 4> makeFractionalDelayCoefficients(double fraction)
	{
		std::array<float, 4> coefficients{};

		for (int j = 0; j < 4; ++j)
		{
			double coefficient = 1.f;

			for (int m = 0; m < 4; ++m)
			{
				if (j == m)
					continue;

				coefficient *= (fraction - static_cast<double>(m)) / (static_cast<double>(j) - m);
			}

			coefficients[j] = static_cast<float>(coefficient);
		}

		return coefficients;
	}

	class Response
	{
	public:

		/**
		 * Builds separate left/right impulse responses 
		 * from successful acoustic paths
		 * 
		 * [param] paths
		 * Successful source-to-listener paths
		 * 
		 * [param] sampleRate
		 * Audio samples per second
		 * 
		 * [param] listener
		 * Listener orientation for spatialization
		 * 
		 * [return] 
		 * Separate impulse responses 
		 * for the left and right output channels
		 * 
		 */
		static audio::ImpulseResponse buildStereo(
			const std::vector<Path>& paths,
			size_t sampleRate,
			const acoustics::Listener& listener)
		{
			audio::ImpulseResponse response;

			// Determine how large the response buffers are
			size_t largestSampleIndex{};

			for (const auto& path : paths)
			{
				if (!path.reachedListener_)
					continue;

				const size_t sampleIndex = timeToSampleIndex(
					path.propagationTime_, 
					sampleRate);

				if (sampleIndex > largestSampleIndex)
					largestSampleIndex = sampleIndex;
			}

			// Resize channels to be on same time span
			response.left_.resize(largestSampleIndex + 1, 0.f);
			response.right_.resize(largestSampleIndex + 1, 0.f);

			audio::Spatializer spatializer;

			for (const auto& path : paths)
			{
				if (!path.reachedListener_)
					continue;

				const size_t sampleIndex = timeToSampleIndex(
					path.propagationTime_,
					sampleRate);

				// Convert path energy into corresponding amplitude
				const float amplitude = std::sqrt(path.energy_);

				// Determine how much belongs to left and right channels
				const audio::StereoGains gains = spatializer.computeGains(path, listener);

				// Apply spatial gains independently
				// Multiple paths may arrive at the same sample,
				// so their contributions are accumulated
				response.left_[sampleIndex] += amplitude * gains.left_;
				response.right_[sampleIndex] += amplitude * gains.right_;
			}

			return response;
		}

		template <typename HRTFSource>
		static audio::ImpulseResponse buildBinaural(
			const std::vector<Path>& paths,
			size_t sampleRate,
			const acoustics::Listener& listener,
			const HRTFSource& source)
		{
			if (!sampleRate)
				throw std::invalid_argument("Sample rate cannot be zero");

			std::vector<Contribution> contributions;
			contributions.reserve(paths.size());

			size_t responseSize{ 1 };

			// Returns integer sample position and fractional remainder
			const auto splitDelay = [sampleRate](double delaySeconds) -> std::pair<size_t, double>
				{
					const double samplePosition = delaySeconds * static_cast<double>(sampleRate);

					if (!std::isfinite(samplePosition) || samplePosition < 0.0)
						throw std::invalid_argument("Invalid acoustic delay.");

					const double integerPart = std::floor(samplePosition);

					return std::pair<size_t, double>{
						static_cast<size_t>(integerPart),
							samplePosition - integerPart
					};
				};

			for (const auto& path : paths)
			{
				// Paths that never reached the listener contribute nothing
				if (!path.reachedListener_)
					continue;

				// Select the HRTF using the direction from the listener
				// toward the source, not the ray's direction of travel
				const auto [azimuth, elevation] = audio::Spatializer::computeAngles(-path.arrivalDir_, listener);
				audio::HRTF hrtf = source.getHRTF(azimuth, elevation);

				// The input audio and HRIRs must use the same sample rate
				if (hrtf.sampleRate_ != sampleRate)
					throw std::invalid_argument("HRTF sample rate does not match output sample rate");

				if (hrtf.left_.empty() || hrtf.right_.empty() || hrtf.left_.size() != hrtf.right_.size())
					throw std::invalid_argument("HRTF must contain both left and right HRIRs");

				// Convert the path's travel time into an audio sample index
				// 
				// Combine propogation time and ear-specific delay before converting
				// to samples. Preserves fractional timing
				const auto [leftStart, leftFraction] = splitDelay(
					static_cast<double>(path.propagationTime_) +
					static_cast<double>(hrtf.leftDelaySec_));

				const auto [rightStart, rightFraction] = splitDelay(
					static_cast<double>(path.propagationTime_) +
					static_cast<double>(hrtf.rightDelaySec_));

				const auto leftCoefficients = makeFractionalDelayCoefficients(leftFraction);
				const auto rightCoefficients = makeFractionalDelayCoefficients(rightFraction);

				// Convert path energy into an amplitude multiplier for the HRIR
				const float amplitudeGain = std::sqrt(std::max(path.energy_, 0.f));

				responseSize = std::max(responseSize, leftStart + hrtf.left_.size() + 3);
				responseSize = std::max(responseSize, rightStart + hrtf.right_.size() + 3);

				contributions.push_back({ 
					leftStart, rightStart, 
					leftCoefficients, rightCoefficients, 
					amplitudeGain, 
					std::move(hrtf) });

				const size_t leftDelaySamples = timeToSampleIndex(hrtf.leftDelaySec_, sampleRate);
				const size_t rightDelaySamples = timeToSampleIndex(hrtf.rightDelaySec_, sampleRate);

				// Reserve enough samples for the delayed HRIRs to finish
				responseSize = std::max(responseSize, leftStart + hrtf.left_.size() + 3);
				responseSize = std::max(responseSize, rightStart + hrtf.right_.size() + 3);
			}

			audio::ImpulseResponse response;

			response.left_.assign(responseSize, 0.f);
			response.right_.assign(responseSize, 0.f);

			// Accumulate every path's delayed, attenuated HRIR
			for (const auto& ctb : contributions)
			{
				const auto& hrtf = ctb.hrtf_;
				const float gain = ctb.amplitudeGain_;

				for (size_t i = 0; i < hrtf.left_.size(); i++)
				{
					for (size_t k = 0; k < 4; k++)
					{
						response.left_[ctb.leftStart + i + k] += gain * hrtf.left_[i] * ctb.leftFractionalCoefficients[k];
						response.right_[ctb.rightStart + i + k] += gain * hrtf.right_[i] * ctb.rightFractionalCoefficients[k];
					}
				}
			}
			
			return response;
		}

	private:

		// Discrete representation of the acoustic response
		// Each element corresponds to 1 audio sample
		std::vector<float> samples_;

		// Path HRIR and the time at which path arrives
		struct Contribution
		{
			size_t leftStart{};
			size_t rightStart{};

			std::array<float, 4> leftFractionalCoefficients{};
			std::array<float, 4> rightFractionalCoefficients{};

			float amplitudeGain_;

			audio::HRTF hrtf_;
		};
	};
}

