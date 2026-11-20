#pragma once

// HRTF
#include <audio/HRTF.hpp>


namespace kodama::audio
{
	struct HRTFEntry
	{
		// Azimuth relative to the listener (in degrees)
		//
		// 0	= directly in front
		// +90	= right
		// -90	= left
		float azimuth_{};

		// Elevation (in degrees)
		//
		// 0	= horizontal plane
		// +90	= directly above
		// -90	= directly below
		float elevation_{};

		// HRTF associated with this direction
		HRTF hrtf_;
	};
}
