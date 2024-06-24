#include "SwitchFloorButton.h"
#include "Graphics.h"
#include "Input/Input.h"
#include "imgui/imgui.h"
#include "Player.h"
#include "SwitchManager.h"
SwitchFloorButton::SwitchFloorButton()
{
	Graphics& graphics = Graphics::Instance();
	//scale = { 1.0f,1.0f,1.0f };;
	scale = { 0.01f,0.01f,0.01f };
	skinned_meshes = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\SwitchItems\\PTK_ActionField_1.fbx", true, 0, scale.x);

	SwitchManager& manager = SwitchManager::Instance();
	//manager.GetEnemyFromId()
}

SwitchFloorButton::~SwitchFloorButton()
{
}

void SwitchFloorButton::Update(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();

	Player& player = Player::Instance();

	SwitchManager& switchmanager = SwitchManager::Instance();
	
	
	
		if (IsOFF1() && modelSwitchtype == ModelSwitchType::TrapOff && CountType == SwitchCountType::Switch1 && switchmanager.GetFromId(1)->GetId() == 1)
		{
			position.y -= 0.001f;
			if (position.y <= -75.488f)
			{
				modelSwitchtype = ModelSwitchType::TrapOn;
				ON = true;
			}
		}
		if (IsOFF1() && modelSwitchtype == ModelSwitchType::TrapOff && CountType == SwitchCountType::Switch2 && switchmanager.GetFromId(2)->GetId() == 2)
		{
			position.y -= 0.001f;
			if (position.y <= -66.0f)
			{
				modelSwitchtype = ModelSwitchType::TrapOn;
				ON = true;
			}
		}

	UpdateTransform();


}

void SwitchFloorButton::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[], Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[], Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{
	{
		immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);


		skinned_meshes->render(immediate_context.Get(), transform, material_color, nullptr);
	}
}

void SwitchFloorButton::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

void SwitchFloorButton::SetON1(bool isActive)
{
	{ active2 = isActive; }
}

bool SwitchFloorButton::IsOFF1() 
{
	return active2;
}

void SwitchFloorButton::ALLDebugImguiStage()
{

	ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	ImGui::Begin("SwitchFloorStage");
	ImGui::InputFloat3("position", &position.x);
	ImGui::InputFloat3("angle", &angle.x);
	ImGui::InputFloat3("scale", &scale.x);
	ImGui::Checkbox("Flag",&ON);
	// ���݂̃J�E���g�^�C�v��\�����A�I�������h���b�v�_�E���ŕ\��
	const char* items[] = { "Switch1", "Switch2", "Switch3" /* ���̃X�C�b�`����ǉ� */ };
	static int item_current = static_cast<int>(CountType);

	if (ImGui::Combo("Count Type", &item_current, items, IM_ARRAYSIZE(items))) {
		CountType = static_cast<SwitchCountType>(item_current);
	}
	ImGui::Separator();

	ImGui::End();


}

skinned_mesh* SwitchFloorButton::GetModel()
{
	return skinned_meshes.get();
}

void SwitchFloorButton::SetSwitchCountType(SwitchCountType switchCount)
{
	 CountType=switchCount;
}
