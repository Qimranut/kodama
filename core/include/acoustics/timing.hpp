#pragma once

// stds
#include <cmath>
#include <cstddef>


namespace kodama::acoustics
{
	/**
	 * Converts a propagation time in seconds into
	 * the nearest sample position in an audio buffer
	 * 
	 * [param] timeSeconds
	 * Arrival time in seconds
	 * 
	 * [param] sampleRate
	 * Audio samples per second
	 * 
	 * [return] 
	 * Nearest sample index for the given time
	 * 
	 */
	inline size_t timeToSampleIndex(
		float timeSeconds,
		size_t sampleRate)
	{
		// Convert seconds into a floating point sample position
		const float samplePos = timeSeconds * static_cast<float>(sampleRate);

		// Round to the nearest discrete audio sample
		return static_cast<size_t>(std::round(samplePos));
	}
}
