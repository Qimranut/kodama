#pragma once

// stds
#include <numbers>


namespace kodama::math
{
	constexpr double degToRad(double deg)
	{
		return deg * std::numbers::pi / 180.0;
	}

	constexpr double radToDeg(double rad)
	{
		return rad * 180.0 / std::numbers::pi;
	}
}
