#pragma once

#include <vector>
#include "Switch.h"

//�X�e�[�W�}�l�[�W���[
class SwitchManager {

public:

	//�B��̃C���X�^���X�擾
	static SwitchManager& Instance() {
		static SwitchManager instance;
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
	void Register(std::unique_ptr<SwitchALL> switchAll);
	//�X�e�[�W�S�폜
	void Clear();
	
	void DebugImguiStage();
	skinned_mesh* GetModel(skinned_mesh* model);
	// StageMoveFloor�̃��f�����擾
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
