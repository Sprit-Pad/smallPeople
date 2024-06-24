// RAYCAST
#pragma once

#include <directxmath.h>
#include <utility>

int intersect_ray_triangles
(
	const float* positions, // vertices
	const uint32_t offset, // bytes
	const uint32_t stride, // bytes
	const uint32_t* indices,
	const size_t index_count,
	const DirectX::XMFLOAT4& ray_position,
	const DirectX::XMFLOAT4& ray_direction,
	DirectX::XMFLOAT4& intersection,
	float& distance,
	const bool RHS = true
);

// The coordinate systems of all the argument variables of these functions must be the same.
inline bool intersect_ray_aabb(const float p[3], const float d[3], const float min[3], const float max[3])
{
	float tmin{ 0 };
	float tmax{ +FLT_MAX };

	for (size_t a = 0; a < 3; ++a)
	{
		float inv_d{ 1.0f / d[a] };
		float t0{ (min[a] - p[a]) * inv_d };
		float t1{ (max[a] - p[a]) * inv_d };
		if (inv_d < 0.0f)
		{
			std::swap<float>(t0, t1);
		}
		tmin = t0 > tmin ? t0 : tmin;
		tmax = t1 < tmax ? t1 : tmax;
		if (tmax <= tmin)
		{
			return false;
		}
	}
	return true;
};

	