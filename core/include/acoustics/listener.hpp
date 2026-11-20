#pragma once

#include <math/vec3.hpp>

namespace kodama::acoustics
{
	struct Listener
	{
		// World space pos
		math::Vec3 pos_{};

		// Listener direction
		// +Z as up, so +Y is default forward direction
		math::Vec3 forward_{ 0.f, 1.f, 0.f };

		// Listener up direction
		math::Vec3 up_{ 0.f, 0.f, 1.f };
		
		// Radius of listener's acoustic capture area
		float radius_{};

		/**
		 * Computes listener's local right direction
		 * 
		 * [return]
		 * 
		 * 
		 */
		math::Vec3 right() const
		{
			return math::normalize(math::cross(forward_, up_));
		}
	};
}
