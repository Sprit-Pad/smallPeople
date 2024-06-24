#pragma once



#include "Stage.h"


class StageMain :public Stage {
public:
	StageMain();
   ~StageMain()override;
  
  	void Update(float elapsedTime)override;
  	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)override;
	void UpdateTransform();
	bool Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial);
	void ALLDebugImguiStage()override;
	skinned_mesh* GetModel()override;
	skinned_mesh* GetModel1()override;
	ModelType GetModelType() const override { return ModelType::StageMain; } // ’Ç‰Á
	static StageMain& Instance() {
		static StageMain instance;
		return instance;
	}

private:

	//FBXSDK
	std::unique_ptr<skinned_mesh> skinned_meshes;
	DirectX::XMFLOAT3 scale = { 10.0f,10.0f,10.0f };
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	DirectX::XMFLOAT4X4 island_transform{ -0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 1 };
	ModelType modeltype = ModelType::StageMain;
};