#pragma once

// Vec3
#include <math/vec3.hpp>


namespace kodama::geometry
{
	struct Ray
	{
		math::Vec3 origin_;
		math::Vec3 dir_;
	};
}
