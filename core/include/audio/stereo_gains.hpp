#pragma once

namespace kodama::audio
{
	struct StereoGains
	{
		// Gain applied to the left channel
		float left_{};

		// Gain applied to the right channel
		float right_{};
	};
}
