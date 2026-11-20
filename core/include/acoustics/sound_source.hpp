#pragma once

// Vec3
#include <math/vec3.hpp>


namespace kodama::acoustics
{
	struct SoundSource
	{
		// Position of sound source
		math::Vec3 pos_;

		// Base amount of acoustic energy emitted by source
		float strength_{ 1.f };

		// Distance at which the source's strength is 
		// considered to be its full reference value
		float referenceDist_{ 1.f };
	};
}
