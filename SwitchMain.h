#pragma once
#include "Switch.h"

class SwitchMain :public SwitchALL {
public:
	SwitchMain();
	~SwitchMain()override;

	void Update(float elapsedTime)override;
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)override;

	void UpdateTransform();
	void ALLDebugImguiStage()override;

	void SetON1(bool isActive)override;
	bool IsOFF1() override;

	skinned_mesh* GetModel()override;
	ModelSwitch GetSwitchType()const override { return modelswitch; }
	ModelSwitchType GetModelType() const override { return modelSwitchtype; } // ’Ç‰Á
	SwitchCountType GetSwitchCountType()const override { return SwitchCountType::SwitchMain; }

	void SetSwitchCountType(SwitchCountType switchCount)override;

	const DirectX::XMFLOAT3& GetPosition() { return position; }
	void SetPosition(const DirectX::XMFLOAT3& position)override { this->position = position; }
	static SwitchMain& Instance() {
		static SwitchMain instance;
		return instance;
	}

private:

	//FBXSDK
	std::unique_ptr<skinned_mesh> skinned_meshes;
	DirectX::XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	ModelSwitchType modelSwitchtype = ModelSwitchType::TrapOff;
	ModelSwitch modelswitch = ModelSwitch::Bittkuri; 
	bool active2;
};