#pragma once

// stds
#include <cstddef>


namespace kodama::acoustics
{
	struct Arrival
	{
		// Arrival time in seconds
		float time_{};

		// Arrival position in the audio timeline
		size_t sampleIndex_{};

		// Energy arriving at the listener
		float energy_{};

		// Reflection count
		int bounceCount_{};
	};
}
