// RAYCAST
#include "raycast.h"

using namespace DirectX;


int intersect_ray_triangles
(
	const float* positions, 
	const uint32_t offset, 
	const uint32_t stride, 
	const uint32_t* indices,
	const size_t index_count,
	const XMFLOAT4& ray_position,
	const XMFLOAT4& ray_direction,
	XMFLOAT4& intersection,
	float& distance,
	const bool RHS
)
{
	const bool CCW{ RHS }; 
	const int C0{ 0 };
	const int C1{ CCW ? 1 : 2 };
	const int C2{ CCW ? 2 : 1 };

	int intersection_count{ 0 };
	int intersected_triangle_index{ -1 };

	const float ray_length_limit{ distance };
	float closest_distance{ FLT_MAX };

	XMVECTOR X{}; // closest cross point
	const XMVECTOR P{ XMVectorSet(ray_position.x, ray_position.y, ray_position.z, 1) };
	const XMVECTOR D{ XMVector3Normalize(XMVectorSet(ray_direction.x, ray_direction.y, ray_direction.z, 0)) };

	using byte = uint8_t;
	const byte* p{ reinterpret_cast<const byte*>(positions) + offset };
	const size_t triangle_count{ index_count / 3 };
	for (size_t triangle_index = 0; triangle_index < triangle_count; triangle_index++)
	{
		const XMVECTOR A{ XMVectorSet(
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[0],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[1],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C0] * stride))[2],
			1.0f
		) };
		const XMVECTOR B{ XMVectorSet(
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[0],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[1],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C1] * stride))[2],
			1.0f
		) };
		const XMVECTOR C{ XMVectorSet(
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[0],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[1],
			(reinterpret_cast<const float*>(p + indices[triangle_index * 3 + C2] * stride))[2],
			1.0f
		) };

		XMVECTOR Q;

		const XMVECTOR N{ XMVector3Normalize(XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A))) };
		const float d{ XMVectorGetByIndex(XMVector3Dot(N, A), 0) };

		const float denominator{ XMVectorGetByIndex(XMVector3Dot(N, D), 0) };
		if (denominator < 0) 
		{
			const float numerator{ d - XMVectorGetByIndex(XMVector3Dot(N, P), 0) };
			const float t{ numerator / denominator };

			if (t > 0 && t < ray_length_limit) 
			{
				Q = XMVectorAdd(P, XMVectorScale(D, t));

				const XMVECTOR QA{ XMVectorSubtract(A, Q) };
				const XMVECTOR QB{ XMVectorSubtract(B, Q) };
				const XMVECTOR QC{ XMVectorSubtract(C, Q) };

				XMVECTOR U{ XMVector3Cross(QB, QC) };
				XMVECTOR V{ XMVector3Cross(QC, QA) };
				if (XMVectorGetByIndex(XMVector3Dot(U, V), 0) < 0)
				{
					continue;
				}

				XMVECTOR W{ XMVector3Cross(QA, QB) };
				if (XMVectorGetByIndex(XMVector3Dot(U, W), 0) < 0)
				{
					continue;
				}
				if (XMVectorGetByIndex(XMVector3Dot(V, W), 0) < 0)
				{
					continue;
				}

				// Otherwise Q must be in (or on) the triangle
				if (t < closest_distance)
				{
					closest_distance = t;
					intersected_triangle_index = static_cast<int>(triangle_index);
					X = Q;
				}
				intersection_count++;
			}
		}
	}
	if (intersection_count > 0)
	{
		XMStoreFloat4(&intersection, X);
		distance = closest_distance;
	}
	return intersected_triangle_index;
}
	int intersect_ray_triangle
	(
		const float* positions,
		const uint32_t offset,
		const uint32_t stride,
		const uint32_t* indices,
		const size_t index_count,
		const XMFLOAT4& ray_position,
		const XMFLOAT4& ray_direction,
		XMFLOAT4& intersection,
		float& distance,
		const bool RHS
	)
	{
		const bool CCW{ RHS };
		const int C0{ 0 };
		const int C1{ CCW ? 1 : 2 };
		const int C2{ CCW ? 2 : 1 };

		int intersection_count{ 0 };
		int intersected_triangle_index{ -1 };

		const float ray_length_limit{ distance };
		float closest_distance{ FLT_MAX };

		XMVECTOR X{}; // closest cross point
		const XMVECTOR P{ XMLoadFloat4(&ray_position) };
		const XMVECTOR D{ XMVector3Normalize(XMLoadFloat4(&ray_direction)) };

		using byte = uint8_t;
		const byte* p{ reinterpret_cast<const byte*>(positions) + offset };
		const size_t triangle_count{ index_count / 3 };
		for (size_t triangle_index = 0; triangle_index < triangle_count; triangle_index++)
		{
			const XMVECTOR A{ XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(p + indices[triangle_index * 3 + C0] * stride)) };
			const XMVECTOR B{ XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(p + indices[triangle_index * 3 + C1] * stride)) };
			const XMVECTOR C{ XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(p + indices[triangle_index * 3 + C2] * stride)) };

			XMVECTOR Q;

			const XMVECTOR N{ XMVector3Normalize(XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A))) };
			const float d{ XMVectorGetX(XMVector3Dot(N, A)) };

			const float denominator{ XMVectorGetX(XMVector3Dot(N, D)) };
			if (denominator < 0)
			{
				const float numerator{ d - XMVectorGetX(XMVector3Dot(N, P)) };
				const float t{ numerator / denominator };

				if (t > 0 && t < ray_length_limit)
				{
					Q = XMVectorAdd(P, XMVectorScale(D, t));

					const XMVECTOR QA{ XMVectorSubtract(A, Q) };
					const XMVECTOR QB{ XMVectorSubtract(B, Q) };
					const XMVECTOR QC{ XMVectorSubtract(C, Q) };

					XMVECTOR U{ XMVector3Cross(QB, QC) };
					XMVECTOR V{ XMVector3Cross(QC, QA) };
					if (XMVectorGetX(XMVector3Dot(U, V)) < 0)
					{
						continue;
					}

					XMVECTOR W{ XMVector3Cross(QA, QB) };
					if (XMVectorGetX(XMVector3Dot(U, W)) < 0)
					{
						continue;
					}
					if (XMVectorGetX(XMVector3Dot(V, W)) < 0)
					{
						continue;
					}

					// Otherwise Q must be in (or on) the triangle
					const float new_closest_distance{ t };
					if (new_closest_distance < closest_distance)
					{
						closest_distance = new_closest_distance;
						intersected_triangle_index = static_cast<int>(triangle_index);
						X = Q;
					}
					intersection_count++;
				}
			}
		}
		if (intersection_count > 0)
		{
			XMStoreFloat4(&intersection, X);
			distance = closest_distance;
		}
		return intersected_triangle_index;
	}
	bool intersect_ray_trianglebool(const XMFLOAT4& rayPosition, const XMFLOAT4& rayDirection,
		const XMFLOAT3& vertexA, const XMFLOAT3& vertexB, const XMFLOAT3& vertexC,
		XMFLOAT4& intersection, float& distance)
	{
		const XMVECTOR P = XMLoadFloat4(&rayPosition);
		const XMVECTOR D = XMVector3Normalize(XMLoadFloat4(&rayDirection));

		const XMVECTOR A = XMLoadFloat3(&vertexA);
		const XMVECTOR B = XMLoadFloat3(&vertexB);
		const XMVECTOR C = XMLoadFloat3(&vertexC);

		// Compute triangle normal
		const XMVECTOR N = XMVector3Normalize(XMVector3Cross(XMVectorSubtract(B, A), XMVectorSubtract(C, A)));

		// Compute plane equation (Ax + By + Cz + D = 0) of the triangle
		const float D_plane = -XMVectorGetX(XMVector3Dot(N, A));

		// Check if ray and triangle are parallel
		const float denominator = XMVectorGetX(XMVector3Dot(N, D));
		if (fabs(denominator) < 0.0001f)
		{
			// Ray and triangle are parallel, no intersection
			return false;
		}

		// Compute distance along ray to intersection point
		const float t = -(XMVectorGetX(XMVector3Dot(N, P)) + D_plane) / denominator;

		// Check if intersection point is behind ray origin
		if (t < 0)
		{
			// Intersection point is behind ray origin, no intersection
			return false;
		}

		// Compute intersection point
		const XMVECTOR intersectionPoint = XMVectorAdd(P, XMVectorScale(D, t));
		XMStoreFloat4(&intersection, intersectionPoint);

		// Compute barycentric coordinates to check if intersection point is inside the triangle
		const XMVECTOR Q = XMVectorSubtract(intersectionPoint, A);
		const float dotABAB = XMVectorGetX(XMVector3Dot(XMVectorSubtract(B, A), XMVectorSubtract(B, A)));
		const float dotABAC = XMVectorGetX(XMVector3Dot(XMVectorSubtract(B, A), XMVectorSubtract(C, A)));
		const float dotQAB = XMVectorGetX(XMVector3Dot(Q, XMVectorSubtract(B, A)));
		const float dotQAC = XMVectorGetX(XMVector3Dot(Q, XMVectorSubtract(C, A)));
		const float invDenominator = 1.0f / (dotABAB * dotABAC - dotQAB * dotQAC);
		const float u = (dotABAC * dotQAB - dotABAB * dotQAC) * invDenominator;
		const float v = (dotABAB * dotQAC - dotABAC * dotQAB) * invDenominator;

		// Check if intersection point is inside the triangle
		if (u >= 0 && v >= 0 && (u + v) <= 1)
		{
			// Intersection point is inside the triangle
			distance = t;
			return true;
		}

		// Intersection point is outside the triangle
		return false;
	}