#pragma once

#include <vector>
#include "Stage.h"



//�X�e�[�W�}�l�[�W���[
class StageManager {

public:
	
	//�B��̃C���X�^���X�擾
	static StageManager& Instance() {
		static StageManager instance;
		return instance;
	}

	//�X�V����
	void Update(float elapsedTime);
	//�`�揈��
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color);
	//�X�e�[�W�o�^
	void Register(Stage* stage);
	//�X�e�[�W�S�폜
	void Clear();
	bool Raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction, DirectX::XMFLOAT4& hitPoint, DirectX::XMFLOAT3& hitNormal, std::string& hitMesh, std::string& hitMaterial,ModelType modeltype, bool r);
	void DebugImguiStage();
	skinned_mesh* GetModel(skinned_mesh* model);
	// StageMoveFloor�̃��f�����擾
	skinned_mesh* GetModel1(ModelType modelType);
	Stage* GetStage(int index) { return  stages.at(index); }
	int GetStagesCount()const { return static_cast<int>(stages.size()); }
private:
	std::vector<Stage*> stages;

	


};
