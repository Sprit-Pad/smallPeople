#pragma once

#include <DirectXMath.h>
#include "skinned_mesh.h"

//�R���W����
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0,0,0 };//���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3 normal = { 0,0,0 };//�Փ˂����|���S���̖@���x�N�g��
	DirectX::XMFLOAT3 rotation = { 0,0,0 };//��]��	
	float             distance = 0.0f;//���C�̎��_�����_�܂ł̋���
	int               materialIndex = -1;//�Փ˂����|���S���̃}�e���A���ԍ�
	DirectX::XMFLOAT3	triangleVerts[3];
};
// �����̒�`
struct Ray
{
	DirectX::XMFLOAT3	p;	// �����̎n�_
	DirectX::XMFLOAT3	d;	// �����̕���
	float				l;	// �����̒���
};
// ���ʂ̒�`
struct Plane
{
	DirectX::XMFLOAT3	n;	// �@��
	float				d;	// ���_����̍ŒZ����
};
// ���̒�`
struct Sphere
{
	DirectX::XMFLOAT3	c;	// ���S
	float				r;	// ���a
};
// AABB�̒�`
//struct AABB
//{
//	DirectX::XMFLOAT3	c;	// ���S
//	DirectX::XMFLOAT3	r;	// ���a
//};
struct MeshBounds
{
	DirectX::XMFLOAT3 min; // ���b�V���̋��E�{�b�N�X�̍ŏ��_
	DirectX::XMFLOAT3 max; // ���b�V���̋��E�{�b�N�X�̍ő�_
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
	//�e�ƒe�̌�������
	

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







