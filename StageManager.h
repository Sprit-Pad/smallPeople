#pragma once

#include <vector>
#include "Stage.h"



//ステージマネージャー
class StageManager {

public:
	
	//唯一のインスタンス取得
	static StageManager& Instance() {
		static StageManager instance;
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
	void Register(Stage* stage);
	//ステージ全削除
	void Clear();
	bool Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial,ModelType modeltype, bool r);
	void DebugImguiStage();
	skinned_mesh* GetModel(skinned_mesh* model);
	// StageMoveFloorのモデルを取得
	skinned_mesh* GetModel1(ModelType modelType);
	Stage* GetStage(int index) { return  stages.at(index); }
	int GetStagesCount()const { return static_cast<int>(stages.size()); }
private:
	std::vector<Stage*> stages;

	


};
