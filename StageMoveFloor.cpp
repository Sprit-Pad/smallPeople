#include "StageMoveFloor.h"
#include "Graphics.h"
#include "Input/Input.h"
#include "imgui/imgui.h"

StageMoveFloor::StageMoveFloor()
{
	scale = { 3.0f,1.0f,3.0f };
	Graphics& graphics = Graphics::Instance();
	skinned_meshes = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\Stage\\Cube.fbx", true, 0,scale.x);
	skinned_meshes->SetModelType(ModelType::StageMoveFloor);
	SetActiveType(false);
}

StageMoveFloor::~StageMoveFloor()
{
}

void StageMoveFloor::Update(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	oldTransform = transform;
	oldAngle = angle;
	DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR Goal = DirectX::XMLoadFloat3(&goal);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Goal, Start);
	DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);

	float length;
	DirectX::XMStoreFloat(&length, Length);

	//スタートからゴールまでの間を一秒間で進む割合(0.0~1.0)を算出する
	float speed = moveSpeed * elapsedTime;
	float speedRate = speed / length;
	moveRate += speedRate;
	
	//ゴールに到達、またはスタートに戻った場合、反転	
	if (moveRate <= 0.0f || moveRate >= 1.0f)
	{
		moveSpeed = -moveSpeed;
	}
	
	//線形補完で位置を算出する
	DirectX::XMVECTOR Position = DirectX::XMVectorLerp(Start, Goal, moveRate);
	DirectX::XMStoreFloat3(&position, Position);
	
	//回転
	angle.x += torque.x * elapsedTime;
	angle.y += torque.y * elapsedTime;
	angle.z += torque.z * elapsedTime;

	//行列更新
	UpdateTransform();

	//モデル行列更新
	/*DirectX::XMFLOAT4X4*/ transformIdentity = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	skinned_meshes->dummyMesh = skinned_meshes->meshes;
	for (skinned_mesh::mesh& mesh :skinned_meshes->dummyMesh) {
		mesh.default_global_transform = transformIdentity;
	}
	

}

void StageMoveFloor::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
	, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{
	
	{
		
		immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
		
		{

			skinned_meshes->render(immediate_context.Get(), transform, material_color, nullptr);

		}
	}



}

bool StageMoveFloor::Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial)
{
 //   // 前回のワールド行列と逆行列を求める
    DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&oldTransform);
    DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);

	DirectX::XMFLOAT4 rayDirection{ 0.0f, -1.0f,0.0f, 1.0f }; // 下向きのレイ
	DirectX::XMFLOAT4 rayDirection1 = { position.x,direction .y-1.0f,position.z,direction .w};
	//rayDirection1 = direction;
	//rayDirection1 = rayDirection;
    // 前回のローカル空間でのレイに変換
    DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat4(&position);
    DirectX::XMVECTOR WorldDirection = DirectX::XMLoadFloat4(&rayDirection);

    DirectX::XMVECTOR LocalPosition = DirectX::XMVector3TransformCoord(WorldPosition, InverseWorldTransform);
    DirectX::XMVECTOR LocalDirection = DirectX::XMVector3TransformNormal(WorldDirection, InverseWorldTransform);

    // ローカル空間でのレイとの交点を求める
    DirectX::XMFLOAT4 localPosition, localDirection;
    DirectX::XMStoreFloat4(&localPosition, LocalPosition);
    DirectX::XMStoreFloat4(&localDirection, LocalDirection);

    // ローカル空間でレイキャストを実行
   
    if (/*skinned_meshes[0]->SpatialPartitionedRaycast(localPosition, localDirection, hitPoint, hitNormal)*/skinned_meshes->MoveFloorRaycast(localPosition, localDirection, transform, hitPoint, hitNormal, hitMesh, hitMaterial))
    {
        // ヒットポイントと法線をワールド空間に変換
        WorldTransform = DirectX::XMLoadFloat4x4(&transform);
        DirectX::XMVECTOR LocalHitPoint = DirectX::XMLoadFloat4(&hitPoint);
        DirectX::XMVECTOR WorldHitPoint = DirectX::XMVector3TransformCoord(LocalHitPoint, WorldTransform);

        DirectX::XMVECTOR LocalHitNormal = DirectX::XMLoadFloat3(&hitNormal);
        DirectX::XMVECTOR WorldHitNormal = DirectX::XMVector3TransformNormal(LocalHitNormal, WorldTransform);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(WorldPosition, WorldPosition);
		DirectX::XMVECTOR Dist = DirectX::XMVector3Length(Vec);
        // 変換されたヒットポイントと法線を格納
        DirectX::XMStoreFloat4(&hitPoint, WorldHitPoint);
        DirectX::XMStoreFloat3(&hitNormal, WorldHitNormal);
	//	DirectX::XMStoreFloat(&direction, Dist);
		
		
		ray = true;
        return true;
    }
	
	
	ray = false;
    return false;
}



void StageMoveFloor::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x,position.y,position.z);
	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);


}
void StageMoveFloor::ALLDebugImguiStage()
{

	/*ImGui::SetNextWindowPos(ImVec2(20, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);


	ImGui::Begin("MoveStage");
	ImGui::InputFloat3("position", &position.x);
	ImGui::InputFloat3("angle", &angle.x);
	ImGui::InputFloat3("scale", &scale.x);
	ImGui::InputFloat4("transform1", &transform._11);
	
	ImGui::Separator();
	ImGui::End();
	ImGui::Begin("MoveStageBool");
	ImGui::Checkbox("ray",&ray);
	ImGui::Separator();
	ImGui::End();*/





}

skinned_mesh* StageMoveFloor::GetModel()
{
	return skinned_meshes.get();
}

skinned_mesh* StageMoveFloor::GetModel1()
{
	return skinned_meshes.get();
}


