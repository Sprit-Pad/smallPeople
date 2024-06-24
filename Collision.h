#pragma once

#include <DirectXMath.h>
#include "skinned_mesh.h"

//コリジョン
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };//レイとポリゴンの交点
	DirectX::XMFLOAT3 normal = { 0,0,0 };//衝突したポリゴンの法線ベクトル
	DirectX::XMFLOAT3 rotation = { 0,0,0 };//回転量	
	float             distance = 0.0f;//レイの視点から交点までの距離
	int               materialIndex = -1;//衝突したポリゴンのマテリアル番号
	DirectX::XMFLOAT3	triangleVerts[3];
};
// 光線の定義
struct Ray
{
	DirectX::XMFLOAT3	p;	// 光線の始点
	DirectX::XMFLOAT3	d;	// 光線の方向
	float				l;	// 光線の長さ
};
// 平面の定義
struct Plane
{
	DirectX::XMFLOAT3	n;	// 法線
	float				d;	// 原点からの最短距離
};
// 球の定義
struct Sphere
{
	DirectX::XMFLOAT3	c;	// 中心
	float				r;	// 半径
};
// AABBの定義
//struct AABB
//{
//	DirectX::XMFLOAT3	c;	// 中心
//	DirectX::XMFLOAT3	r;	// 半径
//};
struct MeshBounds
{
	DirectX::XMFLOAT3 min; // メッシュの境界ボックスの最小点
	DirectX::XMFLOAT3 max; // メッシュの境界ボックスの最大点
};
struct Triangle
{
	DirectX::XMFLOAT3	p0;
	DirectX::XMFLOAT3	p1;
	DirectX::XMFLOAT3	p2;
};


class Collision
{

public:
	//弾と弾の交差判定
	

	static bool IntersectCylinderVsCylinder(
		const DirectX::XMFLOAT3& positionA,
		float radiusA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float radiusB,
		float heightB,
		DirectX::XMFLOAT3& outPositionB
	);

	static bool IntersectSphereVsCylinder(const DirectX::XMFLOAT3& spherePosition,
		float shpereRadius,
		const DirectX::XMFLOAT3& cylinderPosition,
		float cylinderRadius,
		float cylinderHeight,
		DirectX::XMFLOAT3& outCylinderPosition);

	static bool IntersectSphereVsIntersectSphere(const DirectX::XMFLOAT3& spherePosition,
		float shpereRadius, const DirectX::XMFLOAT3& spherePosition1,
		float shpereRadius1,
		DirectX::XMFLOAT3& outCylinderPosition);

		
	static bool IntersectRayVsModel(const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const skinned_mesh*model,
		HitResult&result);


	bool CollideRayVsTriangle(DirectX::XMFLOAT3* p, const Ray& r, const Triangle& t);


	
	static bool IntersectRayVsTriangle(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayDirection, float rayDist, const DirectX::XMFLOAT3 triangleVerts[3], HitResult& result);
 const	static bool spaceDivision = true;

};







