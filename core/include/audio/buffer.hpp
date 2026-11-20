#pragma once

// stds
#include <vector>
#include <cstddef>
#include <utility>
#include <stdexcept>


namespace kodama::audio
{
	class Buffer
	{
	public:

		/**
		 * Create an empty audio buffer
		 * 
		 * [param] sampleRate
		 * Samples per second
		 * 
		 * [param] channelCount
		 * Number of audio channels
		 * 
		 */
		Buffer(size_t sampleRate, size_t channelCount)
			:
			sampleRate_{ sampleRate },
			channelCount_{ channelCount }
		{
			if (!sampleRate || !channelCount)
				throw std::invalid_argument("Audio Buffer requires a non-zero sample rate and channel count");
		}

		/**
		 * Create an audio buffer with existing samples
		 * 
		 * [param] samples
		 * Interleaved audio sample data
		 * 
		 * [param] sampleRate
		 * Samples per second
		 * 
		 * [param] channelCount
		 * Number of audio channels
		 * 
		 */
		Buffer(std::vector<float> samples, size_t sampleRate, size_t channelCount)
			:
			samples_(std::move(samples)),
			sampleRate_{ sampleRate },
			channelCount_{ channelCount }
		{
			if (!sampleRate || !channelCount)
				throw std::invalid_argument("Audio Buffer requires a non-zero sample rate and channel count");

			if (samples_.size() % channelCount != 0)
				throw std::invalid_argument("Audio Buffer sample count must be divisible by channel count");
		}

		// Accessors

		size_t sampleCount() const
		{
			return samples_.size();
		}

		size_t sampleRate() const
		{
			return sampleRate_;
		}

		size_t channelCount() const
		{
			return channelCount_;
		}

		size_t frameCount() const
		{
			return samples_.size() / channelCount_;
		}

		double duration() const
		{
			return static_cast<double>(frameCount()) /
				static_cast<double>(sampleRate_);
		}

		std::vector<float>& samples()
		{
			return samples_;
		}

		const std::vector<float>& samples() const
		{
			return samples_;
		}

	private:

		std::vector<float> samples_;

		size_t sampleRate_;
		size_t channelCount_;

	};
}
