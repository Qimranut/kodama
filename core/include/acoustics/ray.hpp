#pragma once

// Ray
#include <geometry/ray.hpp>


namespace kodama::acoustics
{
	struct Ray
	{
		// Geometric portion of the ray
		geometry::Ray ray_;

		// Amount of sound energy carried by this ray
		//
		// 1.0 = full energy
		// 0.0 = no energy
		float energy_{ 1.f };

		// Frequency of the ray
		// Unit: Hz
		float frequency_{ 1000.f };
	};
}
