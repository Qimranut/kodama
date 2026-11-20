#pragma once

// Vec3
#include <math/vec3.hpp>

// stds
#include <vector>
#include <cstddef>


namespace kodama::acoustics
{
	struct Point
	{
		// Path point position
		math::Vec3 pos_{};

		// Surface normal at this point
		math::Vec3 normal_{};

		// Material ID
		size_t matID_{ 0 };
	};

	struct Path
	{
		// Every checkpoint along the sound path
		std::vector<Point> points_;

		// Total distance travelled by the sound
		float totalDist_{};

		// Sound energy remaining when path reaches the listener
		float energy_{};

		// Number of surface reflections along the path
		int bounceCount_{};

		// Check for path reaching the listener
		bool reachedListener_{ false };

		// Total time required for sound to travel the path
		// from source to listener
		float propagationTime_{};

		// Direction of final ray segment at listener
		math::Vec3 arrivalDir_{};
	};
}
