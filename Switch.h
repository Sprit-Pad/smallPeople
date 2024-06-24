#pragma once
#include "Collision.h"

class SwitchALL
{
public:
	SwitchALL() : active1(false) {};
	virtual ~SwitchALL() {};

	virtual	void Update(float elapsedTime) = 0;
	virtual	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color) = 0;
	//ˆÊ’uŽæ“¾
	virtual const DirectX::XMFLOAT3& GetPosition()= 0;
	//ˆÊ’uÝ’è
	virtual void SetPosition(const DirectX::XMFLOAT3& position) = 0;//{ this->position = position; }
	virtual skinned_mesh* GetModel() = 0;
	virtual void ALLDebugImguiStage() = 0;
	void SetActive1(bool isActive) { active1 = isActive; }
	bool IsActive1() const { return active1; }
	virtual void SetON1(bool isActive) = 0;
	virtual bool IsOFF1() = 0;
	virtual	 ModelSwitchType GetModelType()const = 0;
	virtual	 ModelSwitch GetSwitchType()const = 0;
	virtual	 SwitchCountType GetSwitchCountType()const = 0;
	virtual	 void SetSwitchCountType(SwitchCountType switchCount) = 0;
	bool active1;
	bool activeSwitch1=false;
	bool EventSwitch = false;
	int id = 0;
	virtual void	SetId(int id) { this->id = id; }
	virtual int		GetId() { return id; }
};
