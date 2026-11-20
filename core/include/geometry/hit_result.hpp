#pragma once

// Triangle
#include <geometry/triangle.hpp>

// Vec3
#include <math/vec3.hpp>


namespace kodama::geometry
{
	struct HitResult
	{
		// How far along the ray the intersection occurred
		float dist_{};

		// Position where the ray hit the triangle
		math::Vec3 pos_{};

		// Normal of the surface at the hit point
		math::Vec3 normal_{};

		// Triangle hit
		const Triangle* triangle_{ nullptr };
	};
}
