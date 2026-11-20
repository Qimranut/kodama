#pragma once

// stds
#include <cmath>


namespace kodama::math
{
	struct Vec3
	{
		float x_;
		float y_;
		float z_;
	};

	inline Vec3 operator-(const Vec3& v)
	{
		return
		{
			-v.x_,
			-v.y_,
			-v.z_
		};
	}

	inline Vec3 operator+(const Vec3& a, const Vec3& b)
	{
		return
		{
			a.x_ + b.x_,
			a.y_ + b.y_,
			a.z_ + b.z_
		};
	}

	inline Vec3 operator-(const Vec3& a, const Vec3& b)
	{
		return
		{
			a.x_ - b.x_,
			a.y_ - b.y_,
			a.z_ - b.z_
		};
	}

	inline Vec3 operator*(const Vec3& v, float scalar)
	{
		return
		{
			v.x_ * scalar,
			v.y_ * scalar,
			v.z_ * scalar
		};
	}

	inline float dot(const Vec3& a, const Vec3& b)
	{
		return	a.x_ * b.x_ + 
				a.y_ * b.y_ + 
				a.z_ * b.z_;
	}

	inline float length(const Vec3& v)
	{
		return std::sqrt(dot(v, v));
	}

	inline Vec3 normalize(const Vec3& v)
	{
		const float len = length(v);

		return
		{
			v.x_ / len,
			v.y_ / len,
			v.z_ / len
		};
	}

	inline Vec3 reflect(const Vec3& dir, const Vec3& normal)
	{
		return dir - normal * (2.f * dot(dir, normal));
	}

	inline Vec3 cross(const Vec3& a, const Vec3& b)
	{
		return
		{
			a.y_ * b.z_ - a.z_ * b.y_,
			a.z_ * b.x_ - a.x_ * b.z_,
			a.x_ * b.y_ - a.y_ * b.x_,
		};
	}
}
