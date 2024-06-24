#include "SwitchManager.h"

void SwitchManager::Update(float elapsedTime)
{
	for (auto& switch1 : switchs)
	{
		if (switch1->IsActive1()) {
			switch1->Update(elapsedTime);
		}
	}
}

void SwitchManager::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context, Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[], Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[], Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{
	for (auto& switch1 : switchs)
	{
		switch1->Render(immediate_context, rasterizer_states, depth_stencil_states, blend_states, material_color);
	}
}

void SwitchManager::Register(std::unique_ptr<SwitchALL> switchAll)
{
	
    //switchAll->SetId(id + static_cast<int>(SwitchCountType::SwitchMain));
   // id++;
	
	switchs.emplace_back(std::move(switchAll));
	
}

void SwitchManager::Clear()
{
	switchs.clear();
}

void SwitchManager::DebugImguiStage()
{
	for (auto& switch1 : switchs) {
		switch1->ALLDebugImguiStage();
	}
}

skinned_mesh* SwitchManager::GetModel(skinned_mesh* model)
{
	return model;
}

ModelSwitchType* SwitchManager::GetModelType(ModelSwitchType* modelType)
{
	return modelType;
}

SwitchCountType* SwitchManager::GetCountType(SwitchCountType* modelType)
{
	return modelType;
}
// IDからエネミーを取得する
SwitchALL* SwitchManager::GetFromId(int id)
{
	for (auto &Switch :switchs )
	{
		
		if (Switch->GetId() == id)
			return  Switch.get();  // std::unique_ptrから生ポインタを取得
	}
	return nullptr;
}


