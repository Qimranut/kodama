#pragma once

// Audio Buffer
#include <audio/buffer.hpp>

// FFT
#include <audio/FFT.hpp>

// HRTF
#include <audio/HRTF.hpp>

// Impulse Response
#include <audio/impulse_response.hpp>

// stds
#include <vector>
#include <array>
#include <cstddef>
#include <complex>


namespace kodama::audio
{
	class Convolver
	{
	public:

		/**
		 * Performs discrete time-domain convolution
		 * 
		 * [param] input
		 * Original audio samples
		 * 
		 * [param] impulseResponse
		 * Room impulse response samples
		 * 
		 * [return] 
		 * Convolved output signal
		 * 
		 */
		static std::vector<float> convolve(
			const std::vector<float>& input,
			const std::vector<float>& impulseResponse)
		{
			// Linear convolution produces a signal whose length is
			// input length + impulse response length - 1
			if (input.empty() || impulseResponse.empty())
				return {};

			const size_t outputSize = input.size() + impulseResponse.size() - 1;

			std::vector<float> output(outputSize, 0.f);

			for (size_t inputIndex = 0; inputIndex < input.size(); ++inputIndex)
			{
				// Apply impulse response coefficient on every input sample
				for (size_t impulseIndex = 0; impulseIndex < impulseResponse.size(); ++impulseIndex)
				{
					// Coefficient represents how much the input contributes at the delay
					output[inputIndex + impulseIndex] += 
						input[inputIndex] * 
						impulseResponse[impulseIndex];
				}
			}

			return output;
		}
		
		/**
		 * Convolves a mono audio buffer with an impulse response
		 * 
		 * [param] input
		 * Input audio buffer
		 * 
		 * [param] impulseResponse
		 * Acoustic impulse response
		 * 
		 * [return] 
		 */
		Buffer process(
			const Buffer& input, 
			const std::vector<float>& impulseResponse) const
		{
			if (input.channelCount() != 1)
				throw std::invalid_argument("Convolver::process currently requires mono input");

			const auto outputSamples = convolve(input.samples(), impulseResponse);

			return Buffer(outputSamples, input.sampleRate(), input.channelCount());
		}

		/**
		 * Convolves a mono source with a stereo impulse response
		 * 
		 * [param] input
		 * Mono input audio
		 * 
		 * [param] response
		 * Left/right acoustic impulse responses
		 * 
		 * [return] 
		 * Stereo audio Buffer object containing the spatialized audio
		 */
		static audio::Buffer processStereo(
			const audio::Buffer& input,
			const audio::ImpulseResponse& response)
		{
			// A world-space source is currently represented by a single mono signal
			if (input.channelCount() != 1)
				throw std::invalid_argument("processStereo requires mono input");

			// Both channels must represent the same acoustic timeline
			if (response.left_.size() != response.right_.size())
				throw std::invalid_argument("Stereo impulse response channel sizes must match");

			// Convolve the source independently with each channel's acoustic response
			const auto left = convolve(input.samples(), response.left_);
			const auto right = convolve(input.samples(), response.right_);

			// Both convolutions need to have the same length
			if (left.size() != right.size())
				throw std::runtime_error("Stereo convolution produced different output sizes");

			// Set up output buffer
			// Interleave the two channels
			// [L0, R0, L1, R1, ...]
			std::vector<float> output;
			output.resize(left.size() * 2);

			for (size_t i = 0; i < left.size(); i++)
			{
				output[i * 2] = left[i];
				output[i * 2 + 1] = right[i];
			}

			return Buffer(std::move(output), input.sampleRate(), 2);
		}

		static std::array<std::vector<float>, 2> processHRTF(
			const std::vector<float>& input,
			const HRTF& hrtf)
		{
			// Both channels must represent the same acoustic timeline
			if (hrtf.left_.size() != hrtf.right_.size())
				throw std::invalid_argument("Stereo impulse response channel sizes must match");

			// Apply directional HRTF to each ear
			const std::vector<float> left = convolve(input, hrtf.left_);
			const std::vector<float> right = convolve(input, hrtf.right_);

			return { left, right };
		}

		/**
		* Performs linear convolution using FFTs
		*
		* [param] input
		* Input audio samples
		*
		* [param] impulseResponse
		* Acoustic impulse response
		*
		* [return]
		* Convolved output samples
		*
		*/
		std::vector<float> convolveFFT(
			const std::vector<float>& input,
			const std::vector<float>& impulseResponse) const
		{
			if (input.empty() || impulseResponse.empty())
				return {};

			// Linear convolution produces N + M - 1 samples
			const size_t outputSize = input.size() + impulseResponse.size() - 1;

			// Find the smallest power of two that can contain
			// the complete linear-convolution result
			size_t fftSize = 1;

			while (fftSize < outputSize)
				fftSize <<= 1;

			std::vector<std::complex<float>> inputSpectrum(fftSize, { 0.f, 0.f });

			std::vector<std::complex<float>> impulseSpectrum(fftSize, { 0.f, 0.f });

			// Copy real input samples into the complex FFT buffers
			for (size_t i = 0; i < input.size(); ++i)
				inputSpectrum[i] = input[i];

			for (size_t i = 0; i < impulseResponse.size(); ++i)
				impulseSpectrum[i] = impulseResponse[i];

			// Transform both signals into the frequency domain
			FFT::transform(inputSpectrum, false);
			FFT::transform(impulseSpectrum, false);

			// Convolution in time becomes multiplication in the frequency domain
			for (size_t i = 0; i < fftSize; ++i)
				inputSpectrum[i] *= impulseSpectrum[i];

			// Transform back into the time domain
			FFT::transform(inputSpectrum, true);

			std::vector<float> output(outputSize);

			// Inverse FFT contains tiny imaginary components caused by
			// float point error. We only need the real component for
			// our real-valued audio signal
			for (size_t i = 0; i < outputSize; ++i)
				output[i] = inputSpectrum[i].real();

			return output;
		}
	};
}
