#pragma once

#include <DirectXMath.h>
#include "../Collision.h"
#include "../skinned_mesh.h"
#include "../ShaderContext.h"

//カメラ
class Camera {

private:
	Camera() {}
	~Camera(){}

public:
	//唯一のインスタンス取得
	static Camera& Instance() 
	{
		static Camera camera;
		return camera;

	}
   
	//指定方向を向く
	void SetLookAt(const DirectX::XMFLOAT4& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	//パースペクティブ設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	void CalculateFrustum(Plane* ret, DirectX::XMFLOAT3* nearP, DirectX::XMFLOAT3* farP, float expandDistance);


	void MixViewProjection( ShaderContext::scene_constants& data,DirectX::XMMATRIX	view, DirectX::XMMATRIX	projection);

	//ビュー行列取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	//プロジェクション行列取得
	const DirectX::XMFLOAT4X4& GetProjection() { return projection; }

	//視点取得
	const DirectX::XMFLOAT4& GetEye()const { return eye; }
	void SetEye(DirectX::XMFLOAT4 Eye) { eye=Eye; }
							   
	//上方向取得				
	const DirectX::XMFLOAT3& GetUp()const { return up; }
							   
	//注視点方向取得				
	const DirectX::XMFLOAT3 &  GetFront()const { return front; }
							   
	//方向取得				
	const DirectX::XMFLOAT3&  GetRight()const { return right; }

	//右方向取得

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
	DirectX::XMFLOAT3	NearP[4] = {};	// Nearの四角形の４頂点の座標
	DirectX::XMFLOAT3	FarP[4] = {}; 	// Farの四角形の４頂点の座標
	DirectX::XMFLOAT3	outLineNorm[4] = {};
	DirectX::XMFLOAT3	cameraPos = {};
	DirectX::XMMATRIX   viewMatrix;
	DirectX::XMMATRIX projectionMatrix;
	float				Near = 3.0f;	// カリングカメラからNearの四角形までの距離
	float				Far = 25.0f;	// カリングカメラからFarの四角形までの距離
	float				NearW = 0.1f;	// カメラからNearの四角形までの距離
	float				FarW = 1000.0f;	// カメラからFarの四角形までの距離

	bool hit2 = false;

	




};









