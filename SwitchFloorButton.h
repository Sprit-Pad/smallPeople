#pragma once
#include "Switch.h"

class SwitchFloorButton :public SwitchALL {
public:
	SwitchFloorButton();
	~SwitchFloorButton()override;

	void Update(float elapsedTime)override;
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)override;

	void UpdateTransform();

	const DirectX::XMFLOAT3& GetPosition() { return position; }
    void SetPosition(const DirectX::XMFLOAT3& position)override { this->position = position; }

	void SetON1(bool isActive)override;
	bool IsOFF1() override;
	bool ActiveONOFF() { return ON; }

	void ALLDebugImguiStage()override;

	skinned_mesh* GetModel()override;
	ModelSwitch GetSwitchType()const override { return modelswitch; }
	ModelSwitchType GetModelType() const override { return modelSwitchtype; } // ’Ç‰Á
	SwitchCountType GetSwitchCountType()const override { return CountType; }

	void SetSwitchCountType(SwitchCountType switchCount)override;

	static SwitchFloorButton& Instance() {
		static SwitchFloorButton instance;
		return instance;
	}

public:

	bool ON = false;

private:

	DirectX::XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	ModelSwitchType modelSwitchtype = ModelSwitchType::TrapOff;
	ModelSwitch modelswitch = ModelSwitch::Yukaswitch;
	SwitchCountType CountType = SwitchCountType::Switch1;
	//FBXSDK
	std::unique_ptr<skinned_mesh> skinned_meshes;
	bool active2;
};
