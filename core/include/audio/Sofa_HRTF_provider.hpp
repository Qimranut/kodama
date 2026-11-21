#pragma once

// HRTF
#include <audio/HRTF.hpp>

// Math
#include <math/util.hpp>

// Sofa
#include <mysofa.h>

// stds
#include <string>
#include <cstddef>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>


namespace kodama::audio
{
	class SofaHRTFProvider
	{
	public:

		SofaHRTFProvider() = default;

		~SofaHRTFProvider()
		{
			if (easy_ != nullptr)
				mysofa_close(easy_);
		}

		SofaHRTFProvider(const SofaHRTFProvider&) = delete;
		SofaHRTFProvider& operator=(const SofaHRTFProvider&) = delete;

		/**
		 * Opens a SOFA file at the requested processing sample rate
		 * 
		 * [param] filePath
		 * Path to sofa file
		 * 
		 * [param] sampleRate
		 * Audio sample rate
		 * 
		 * [param] errorMsg
		 * Error message for loading failure
		 * 
		 * [return] 
		 * Returns true if the dataset opens successfully
		 * 
		 */
		bool load(const std::string& filePath, size_t sampleRate, std::string& errorMsg)
		{
			// Sample rate check
			if (!sampleRate)
			{
				errorMsg = "Sample rate cannot be 0.";
				return false;
			}

			int newFilterLen{};
			int errorCode = MYSOFA_OK;

			// Open a candidate first so a failed load 
			// preserves any dataset this provider was already using
			auto* candidate = mysofa_open_no_norm(
				filePath.c_str(),
				static_cast<float>(sampleRate),
				&newFilterLen,
				&errorCode);

			if (candidate == nullptr ||
				errorCode != MYSOFA_OK ||
				newFilterLen <= 0)
			{
				if (candidate != nullptr)
					mysofa_close(candidate);

				errorMsg = "Failed to open SOFA file. libmysofa error code: " + std::to_string(errorCode);

				return false;
			}

			if (easy_ != nullptr)
				mysofa_close(easy_);

			easy_ = candidate;
			filterLen_ = newFilterLen;
			sampleRate_ = sampleRate;

			errorMsg.clear();

			return true;
		}

		HRTF getHRTF(float azimuthDeg, float elevationDeg) const
		{
			if (easy_ == nullptr)
				throw std::logic_error("Cannot query HRTF: No SOFA dataset loaded.");

			if (!std::isfinite(azimuthDeg) ||
				!std::isfinite(elevationDeg) ||
				elevationDeg < -90.f ||
				elevationDeg > 90.f)
			{
				throw std::invalid_argument("Invalid HRTF azimuth or elevation.");
			}

			const float azimuth = math::degToRad(azimuthDeg);
			const float elevation = math::degToRad(elevationDeg);

			// Project convention:
			// +X -> right | +Y -> forward | +Z -> up
			// 
			// SOFA convention:
			// +X -> forward | +Y -> left | +Z -> up
			//
			// Negative sign converts positive-right azimuth into SOFA's positive-left Y direction
			constexpr float queryRadiusMeters = 1.f;

			const float x = queryRadiusMeters * std::cos(elevation) * std::cos(azimuth);
			const float y = -queryRadiusMeters * std::cos(elevation) * std::sin(azimuth);
			const float z = queryRadiusMeters * std::sin(elevation);

			HRTF hrtf;
			hrtf.sampleRate_ = sampleRate_;
			hrtf.left_.resize(static_cast<size_t>(filterLen_));
			hrtf.right_.resize(static_cast<size_t>(filterLen_));

			float leftDelaySec{};
			float rightDelaySec{};

			// Retrieve both filters and their additional delays
			// The returned delays are expressed in seconds
			mysofa_getfilter_float(
				easy_,
				x, y, z,
				hrtf.left_.data(),
				hrtf.right_.data(),
				&leftDelaySec,
				&rightDelaySec);

			hrtf.leftDelaySec_ = leftDelaySec;
			hrtf.rightDelaySec_ = rightDelaySec;

			return hrtf;
		}

		/**
		 * Prints the n measured positions nearest to a Cartesian query point.
		 * 
		 */
		void dumpNearest(float x, float y, float z, int n = 6) const
		{
			const MYSOFA_HRTF* h = easy_->hrtf;      // positions are Cartesian after mysofa_open
			const float* p = h->SourcePosition.values;
			const unsigned M = h->M;

			std::vector<std::pair<float, unsigned>> d;
			for (unsigned i = 0; i < M; ++i)
			{
				const float dx = p[i * 3] - x, dy = p[i * 3 + 1] - y, dz = p[i * 3 + 2] - z;
				d.push_back({ dx * dx + dy * dy + dz * dz, i });
			}
			std::sort(d.begin(), d.end());

			std::cout << "query (" << x << "," << y << "," << z << ")  M=" << M << '\n';
			for (int k = 0; k < n && k < (int)d.size(); ++k)
			{
				const unsigned i = d[k].second;
				std::cout << "  #" << i << " pos=(" << p[i * 3] << "," << p[i * 3 + 1] << "," << p[i * 3 + 2]
					<< ") dist=" << std::sqrt(d[k].first) << '\n';
			}
		}

		/**
		 * Checks if dataset is loaded
		 * 
		 * [return] 
		 * Returns true if dataset is loaded
		 * 
		 */
		bool isLoaded() const noexcept
		{
			return easy_ != nullptr;
		}

	private:

		MYSOFA_EASY* easy_{};
		int filterLen_{};
		size_t sampleRate_{};
	};
}
