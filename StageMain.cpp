
#include "StageMain.h";
#include "Graphics.h"
#include "Input/Input.h"
#include "imgui/imgui.h"


//コンストラクタ
StageMain::StageMain()
{
	Graphics& graphics = Graphics::Instance();
	scale = { 0.07f,0.07f,0.07f };
	//scale = { 30.5f,30.5f,30.5f };
	skinned_meshes = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\Stage\\uuu.fbx", true, 0, scale.x);
	//skinned_meshes = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\Stage\\LibraryStage.fbx", true, 0, scale.x);
	
}

StageMain::~StageMain()
{
}

void StageMain::Update(float elapsedTime)
{
	UpdateTransform();
	Graphics& graphics = Graphics::Instance();
	
}

void StageMain::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
	, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{

	{
		immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
		

		skinned_meshes->render(immediate_context.Get(), transform, material_color, nullptr);
	}

}

void StageMain::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);
}

bool StageMain::Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial)
{
    return skinned_meshes->SpatialPartitionedRaycast(position, direction, hitPoint, hitNormal);
}

void StageMain::ALLDebugImguiStage()
{
	//ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	//ImGui::Begin("MainStage");
	//
	//ImGui::Separator();

	//ImGui::End();

}

skinned_mesh* StageMain::GetModel()
{
	return skinned_meshes.get();
}

skinned_mesh* StageMain::GetModel1()
{
	return skinned_meshes.get();
}




