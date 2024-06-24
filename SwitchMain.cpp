#include "SwitchMain.h"
#include "Graphics.h"
#include "Input/Input.h"
#include "imgui/imgui.h"

SwitchMain::SwitchMain()
{
	Graphics& graphics = Graphics::Instance();
	scale = { 0.05f,0.05f,0.05f };
	skinned_meshes = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\SwitchItems\\PTK_Exclamation_green.fbx", true, 0, scale.x);
	position.x = -5.897f;
	position.y = -79.111f;
	position.z = -14.929f;
}

SwitchMain::~SwitchMain()
{
}

void SwitchMain::Update(float elapsedTime)
{
	UpdateTransform();
	Graphics& graphics = Graphics::Instance();
	angle.y += elapsedTime;
}

void SwitchMain::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[], Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[], Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{
	{
		immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);


		skinned_meshes->render(immediate_context.Get(), transform, material_color, nullptr);
	}
}

void SwitchMain::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

void SwitchMain::ALLDebugImguiStage()
{
	//ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	//ImGui::Begin("MainStage");
	//
	//ImGui::Separator();

	//ImGui::End();
}

skinned_mesh* SwitchMain::GetModel()
{
	return skinned_meshes.get();
}

void SwitchMain::SetON1(bool isActive)
{
	{ active2 = isActive; }
}

bool SwitchMain::IsOFF1()
{
	return active2;
}

void SwitchMain::SetSwitchCountType(SwitchCountType switchCount)
{
	
}
