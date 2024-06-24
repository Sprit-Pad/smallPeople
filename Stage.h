#pragma once
#include "Collision.h"
#include "ModelType.h"
class Stage {
public:
	Stage() : active(false) {};
	virtual ~Stage() {};

	virtual	void Update(float elapsedTime)=0;
	virtual	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)=0;

	virtual bool Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial)=0;
	virtual void ALLDebugImguiStage()=0;
	void SetActive(bool isActive) { active = isActive; }
	bool IsActive() const { return active; }
	void SetActiveType(bool isActive) { externalFloorBool = isActive; }
	bool IsActiveType() const { return  externalFloorBool; }
	virtual skinned_mesh* GetModel() = 0;
	// ƒ‚ƒfƒ‹‚ÌŽæ“¾
	virtual skinned_mesh* GetModel1() = 0;
	virtual	ModelType GetModelType()const = 0;
	bool active;
	bool externalFloorBool = false;
	int id;
};
	
//extern	bool e1 = true;
//extern	bool e2 = true;