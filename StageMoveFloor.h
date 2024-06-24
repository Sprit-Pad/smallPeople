#pragma once
#include "Stage.h"

//移動床ステージ
class StageMoveFloor :public Stage
{

public:
	StageMoveFloor();
	~StageMoveFloor();

	static StageMoveFloor& Instance() {
		static StageMoveFloor instance;
		return instance;
	}
	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)override;

	bool Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial)override;

	//スタート位置設定
	void SetStartPoint(const DirectX::XMFLOAT3& start) { this->start = start; }
	
	//ゴール位置設定
	void SetGoalPoint(const DirectX::XMFLOAT3& goal) { this->goal = goal; } 

	//トルク設定
	void SetTorque(const DirectX::XMFLOAT3& torque) { this->torque = torque; }

	skinned_mesh* GetModel()override;
	skinned_mesh* GetModel1()override;

	ModelType GetModelType() const override { return ModelType::StageMoveFloor; } // 追加


private:
	//行列更新処理
	void UpdateTransform();

	void ALLDebugImguiStage();

public:
	DirectX::XMFLOAT3 position = { 0,0,0 };

private:
	//FBXSDK
	std::unique_ptr<skinned_mesh> skinned_meshes;
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT4X4  transformIdentity = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT3 torque = { 0,0,0 };
	DirectX::XMFLOAT3 start = { 0,0,0 };
	DirectX::XMFLOAT3 goal = { 0,0,0 };
	DirectX::XMFLOAT4X4 oldTransform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT3 oldAngle = { 0,0,0 };
	float moveSpeed = 2.0f;
	float moveRate = 0.0f;
	bool ray = false;
	ModelType modeltype=ModelType::StageMoveFloor;
};

















