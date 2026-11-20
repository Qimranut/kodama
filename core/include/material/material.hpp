#pragma once

// stds
#include <string>
#include <limits>
#include <algorithm>
#include <cstddef>


namespace kodama::materials
{
	inline constexpr size_t InvalidMatID = std::numeric_limits<size_t>::max();

	struct Material
	{
		// Name of material
		std::string name_;
		
		// Amount of sound energy absorbed by surface material
		//
		// 0.f -> absorbs nothing, reflects everything
		// 1.f -> absorbs everything, reflects nothing
		float absorption_{ 0.f };

		/**
		 * Returns reflected sound energy amount
		 * 
		 * [return] 
		 * Reflected sound energy
		 * 
		 */
		float reflectivity() const
		{
			return 1.f - absorption_;
		}

		/**
		 * Keep the absorption coefficient within
		 * its meaningful range
		 * 
		 */
		void clamp()
		{
			absorption_ = std::clamp(absorption_, 0.f, 1.f);
		}
	};
}
