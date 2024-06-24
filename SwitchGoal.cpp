#include "SwitchGoal.h"
#include "Graphics.h"
#include "Input/Input.h"
#include "imgui/imgui.h"
#include "Player.h"
#include "SwitchManager.h"
SwitchGoal::SwitchGoal()
{
	Graphics& graphics = Graphics::Instance();
	//scale = { 1.0f,1.0f,1.0f };;
	scale = { 0.06f,0.06f,0.06f };
	skinned_meshes = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\SwitchItems\\Goal.fbx", true, 0, scale.x);

	SwitchManager& manager = SwitchManager::Instance();
	
}

SwitchGoal::~SwitchGoal()
{
}

void SwitchGoal::Update(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();

	Player& player = Player::Instance();

	SwitchManager& switchmanager = SwitchManager::Instance();



	if (IsOFF1() && modelSwitchtype == ModelSwitchType::TrapOff && CountType == SwitchCountType::Switch3 && switchmanager.GetFromId(3)->GetId() == 3)
	{
		position.y -= 0.009f;
		if (position.y <= -88.111f)
		{
			modelSwitchtype = ModelSwitchType::TrapOn;
			ON = true;
		}
	}
	

	UpdateTransform();


}

void SwitchGoal::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[], Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[], Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{
	{
		immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);


		skinned_meshes->render(immediate_context.Get(), transform, material_color, nullptr);
	}
}

void SwitchGoal::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

void SwitchGoal::SetON1(bool isActive)
{
	{ active2 = isActive; }
}

bool SwitchGoal::IsOFF1()
{
	return active2;
}

void SwitchGoal::ALLDebugImguiStage()
{

	ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	ImGui::Begin("SwitchFloorStage");
	ImGui::InputFloat3("position", &position.x);
	ImGui::InputFloat3("angle", &angle.x);
	ImGui::InputFloat3("scale", &scale.x);
	ImGui::Checkbox("Flag", &ON);
	// 現在のカウントタイプを表示し、選択肢をドロップダウンで表示
	const char* items[] = { "Switch1", "Switch2", "Switch3" /* 他のスイッチ名を追加 */ };
	static int item_current = static_cast<int>(CountType);

	if (ImGui::Combo("Count Type", &item_current, items, IM_ARRAYSIZE(items))) {
		CountType = static_cast<SwitchCountType>(item_current);
	}
	ImGui::Separator();

	ImGui::End();


}

skinned_mesh* SwitchGoal::GetModel()
{
	return skinned_meshes.get();
}

void SwitchGoal::SetSwitchCountType(SwitchCountType switchCount)
{
	CountType = switchCount;
}
