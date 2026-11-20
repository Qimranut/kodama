#pragma once

// Material
#include <material/material.hpp>

// stds
#include <vector>
#include <cstddef>


namespace kodama::materials
{
	class Registry
	{
	public:

		/**
		 * Registers a material, and returns an ID.
		 * 
		 * [param] mat
		 * [return] 
		 */
		size_t registerMaterial(Material mat)
		{
			mat.clamp();
			materials_.push_back(std::move(mat));
			return materials_.size() - 1;
		}

		/**
		 * Returns number of registered materials.
		 * 
		 * [return] 
		 * Number of registered materials.
		 * 
		 */
		size_t size() const
		{
			return materials_.size();
		}

		// Getter
		const Material& material(size_t matID) const
		{
			return materials_.at(matID);
		}

	private:

		std::vector<Material> materials_;
	};
}
