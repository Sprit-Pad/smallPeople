#pragma once

#include <DirectXMath.h>
#include "../Collision.h"
#include "../skinned_mesh.h"
#include "../ShaderContext.h"

//�J����
class Camera {

private:
	Camera() {}
	~Camera(){}

public:
	//�B��̃C���X�^���X�擾
	static Camera& Instance() 
	{
		static Camera camera;
		return camera;

	}
   
	//�w�����������
	void SetLookAt(const DirectX::XMFLOAT4& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	//�p�[�X�y�N�e�B�u�ݒ�
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	void CalculateFrustum(Plane* ret, DirectX::XMFLOAT3* nearP, DirectX::XMFLOAT3* farP, float expandDistance);


	void MixViewProjection( ShaderContext::scene_constants& data,DirectX::XMMATRIX	view, DirectX::XMMATRIX	projection);

	//�r���[�s��擾
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	//�v���W�F�N�V�����s��擾
	const DirectX::XMFLOAT4X4& GetProjection() { return projection; }

	//���_�擾
	const DirectX::XMFLOAT4& GetEye()const { return eye; }
	void SetEye(DirectX::XMFLOAT4 Eye) { eye=Eye; }
							   
	//������擾				
	const DirectX::XMFLOAT3& GetUp()const { return up; }
							   
	//�����_�����擾				
	const DirectX::XMFLOAT3 &  GetFront()const { return front; }
							   
	//�����擾				
	const DirectX::XMFLOAT3&  GetRight()const { return right; }

	//�E�����擾

	bool MeshFrustumCulling(const skinned_mesh::mesh& mesh/*, const skinned_mesh* model*/);

	bool FrustumCulling(const std::unique_ptr<skinned_mesh>& aabb);


	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;

	DirectX::XMFLOAT4 eye;
	DirectX::XMFLOAT3 focus;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 front;
	DirectX::XMFLOAT3 right;
	float upcamera = 1000;
	float upcamera1 = -300;
	void DebugImGui();
private:


public:
	Plane   frustum[6];
	DirectX::XMFLOAT3	NearP[4] = {};	// Near�̎l�p�`�̂S���_�̍��W
	DirectX::XMFLOAT3	FarP[4] = {}; 	// Far�̎l�p�`�̂S���_�̍��W
	DirectX::XMFLOAT3	outLineNorm[4] = {};
	DirectX::XMFLOAT3	cameraPos = {};
	DirectX::XMMATRIX   viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	float				Near = 3.0f;	// �J�����O�J��������Near�̎l�p�`�܂ł̋���
	float				Far = 25.0f;	// �J�����O�J��������Far�̎l�p�`�܂ł̋���
	float				NearW = 0.1f;	// �J��������Near�̎l�p�`�܂ł̋���
	float				FarW = 1000.0f;	// �J��������Far�̎l�p�`�܂ł̋���

	bool hit2 = false;

	




};









