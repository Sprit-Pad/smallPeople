#include "StageManager.h"

void StageManager::Update(float elapsedTime)
{
	for (Stage* stage : stages) 
	{
        if (stage->IsActive()) {
            stage->Update(elapsedTime);
        }
	}



}

void StageManager::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
	, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{

	for (Stage* stage : stages)
	{
		stage->Render(immediate_context, rasterizer_states, depth_stencil_states, blend_states, material_color);
	}



}

void StageManager::Register(Stage* stage)
{

	stages.emplace_back(stage);

}

void StageManager::Clear()
{

	for (Stage* stage : stages) 
	{

		delete stage;

	}
	stages.clear();

}

bool StageManager::Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial,ModelType modeltype1,bool r)
{
    
    bool hit = false;
    float closest_distance = FLT_MAX;
    DirectX::XMVECTOR Position = DirectX::XMLoadFloat4(&position);
    DirectX::XMVECTOR Direction = DirectX::XMLoadFloat4(&direction);
    DirectX::XMVECTOR NormalizedDirection = DirectX::XMVector3Normalize(Direction);

    for (Stage* stage : stages)
    {
        r = false;
            DirectX::XMFLOAT4 tempHitPoint;
            DirectX::XMFLOAT3 tempHitNormal;
            std::string tempHitMesh;
            std::string tempHitMaterial;
            if (stage->GetModel1()->GetModelType() != modeltype1)continue;
            if (stage->Raycast(position, direction, tempHitPoint, tempHitNormal, tempHitMesh, tempHitMaterial)&&stage->GetModel1()->GetModelType() == modeltype1)
            {
              //  float distance = direction.y; /*sqrt(pow(tempHitPoint.x - position.x, 2) +
               //     pow(tempHitPoint.y - position.y, 2) +
                  //  pow(tempHitPoint.z - position.z, 2));*/

                DirectX::XMVECTOR TempHitPoint = DirectX::XMLoadFloat4(&tempHitPoint);
                DirectX::XMVECTOR DistanceVector = DirectX::XMVectorSubtract(TempHitPoint, Position);
                float distance = DirectX::XMVectorGetX(DirectX::XMVector3Length(DistanceVector));

                if (distance < closest_distance)
                {
                    closest_distance = distance;
                    hitPoint = tempHitPoint;
                    hitNormal = tempHitNormal;
                    hitMesh = tempHitMesh;
                    hitMaterial = tempHitMaterial;
                    hit = true;
                    if (modeltype1 == ModelType::StageMoveFloor)
                    {
                        r = true;
                        stage->SetActiveType(r);
                    }
                    if (modeltype1 == ModelType::StageMain)
                    {
                        r = false;
                        stage->SetActiveType(r);
                    }
                }

            
            }
    }

    return hit;
}

void StageManager::DebugImguiStage()
{
    for (Stage* stage : stages)
    {

        stage->ALLDebugImguiStage();

    }
}

skinned_mesh* StageManager::GetModel(skinned_mesh* model)
{
    

        for (Stage* stage : stages) {

            stage->GetModel();
        }

        return model;
    
}

skinned_mesh* StageManager::GetModel1(ModelType modelType)
{
    for (auto& stage : stages)
    {
        skinned_mesh* model = stage->GetModel();
        if (model && model->GetModelType() == modelType)
        {
            return model;
        }
    }
    return nullptr;
}

