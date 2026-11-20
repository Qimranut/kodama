#pragma once

// stds
#include <vector>


namespace kodama::audio
{
	struct ImpulseResponse
	{
		// Impulse response applied to the left output channel
		std::vector<float> left_;

		// Impulse response applied to the right output channel
		std::vector<float> right_;
	};
}

