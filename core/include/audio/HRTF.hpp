#pragma once

// stds
#include <vector>
#include <cstddef>


namespace kodama::audio
{
	struct HRTF
	{
		// Time-domain response measured at the left ear
		// 
		// An impulse passed through this filter describes how
		// a sound from this direction reaches the left ear
		std::vector<float> left_;

		// Time-domain response measured at the right ear
		std::vector<float> right_;

		// Audio sample rate associated with these HRIRs
		size_t sampleRate_{};

		// Additional delay before the left-ear HRIR
		// Units: Seconds
		float leftDelaySec_{};

		// Additional delay before the right-ear HRIR
		// Units: Seconds
		float rightDelaySec_{};
	};
}
