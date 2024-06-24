#pragma once

#include <vector>
#include "Switch.h"

//ステージマネージャー
class SwitchManager {

public:

	//唯一のインスタンス取得
	static SwitchManager& Instance() {
		static SwitchManager instance;
		return instance;
	}

	//更新処理
	void Update(float elapsedTime);
	//描画処理
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color);
	//ステージ登録
	void Register(std::unique_ptr<SwitchALL> switchAll);
	//ステージ全削除
	void Clear();
	
	void DebugImguiStage();
	skinned_mesh* GetModel(skinned_mesh* model);
	// StageMoveFloorのモデルを取得
	ModelSwitchType* GetModelType(ModelSwitchType* modelType);
	SwitchCountType* GetCountType(SwitchCountType* modelType);

	SwitchALL* GetFromId(int id);
	
	std::unique_ptr<SwitchALL>& GetStage(int index) { return  switchs.at(index); }
	int GetStagesCount()const { return static_cast<int>(switchs.size()); }
private:
	std::vector< std::unique_ptr<SwitchALL>> switchs;

	int id;
	SwitchCountType count=SwitchCountType::Switch1;

};
