#pragma once


#include "Scene.h"
#include "Player.h"
#include "StageMoveFloor.h"
#include "SwitchMain.h"
#include "SwitchFloorButton.h"

// �Q�[���V�[��
class SceneGame :public Scene
{
public:
	SceneGame() {}
	~SceneGame()override {}

	// ������
	void Initialize()override;

	// �I����
	void Finalize()override;

	// �X�V����
	void Update(float elapsed_Time)override;

	// �`�揈��
	void Render()override;
	//�X�e�[�W����
	void EventSystemMovie();

private:

	float ConvertToradians;
	// DirectX11 �ŗ��p����l�X�ȃ��\�[�X���쐬����匳
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	// �L�����o�X�ɕ`��������z�ɓ����
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	// �F���������ސ�̃L�����o�X
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	// ���s�����������ރL�����o�X
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	// �T���v���[�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];
	// �[�x�X�e���V���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];
	// �u�����h�X�e�[�g(0...�����A1...�A���t�@�A2...���Z�A3...��Z)
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];
	//�t���[���o�b�t�@
	std::unique_ptr<framebuffer> framebuffers[8];
	std::unique_ptr<fullscreen_quad> bit_block_transfer;

	std::unique_ptr<sprite> sprites[8];

	std::unique_ptr<sprite_batch> sprite_batches[8];
	//
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
	//
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

public:
	//struct scene_constants
	//{
	//	DirectX::XMFLOAT4X4 view_projection; //�r���[�E�v���W�F�N�V�����ϊ��s��
	//	DirectX::XMFLOAT4 light_direction; //���C�g�̌���
	//	// 
	//	DirectX::XMFLOAT4 camera_position;//�J�����ʒu
	//	//
	//
	//};
	//// �|�X�g�G�t�F�N�g�p�萔�o�b�t�@
	//struct parametric_constants
	//{
	//	float luminance_extraction_lower_edge{ 0.6f };
	//	float luminance_extraction_higher_edge{ 0.8f };
	//	float gaussian_sigma{ 1.0f };
	//	float bloom_intensity{ 1.0f };
	//	float exposure{ 1.0f };
	//	DirectX::XMFLOAT3 dummy2;
	//
	//};
	//parametric_constants parametric_constants;
	//struct PixelShaderConstants
	//{
	//	float bloomIntensity;
	//};
	// �|�X�g�G�t�F�N�g�p�萔�o�b�t�@

	struct Parametric_constants
	{
		float luminance_extraction_lower_edge{ 0.6f };
		float luminance_extraction_higher_edge{ 0.8f };
		float gaussian_sigma{ 1.0f };
		float bloom_intensity{ 1.0f };
		float exposure{ 1.0f };
		DirectX::XMFLOAT3 dummy2;

	};
	Parametric_constants parametric_constants;
	struct PixelShaderConstants
	{
		float bloomIntensity;
	};
	DirectX::XMFLOAT4 options;

	//2DSkyBox
	Microsoft::WRL::ComPtr<ID3D11VertexShader> sky_box_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> sky_box_input_layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> sky_box_pixel_shader;
	std::unique_ptr<sprite> sky_box_render_sprite;

	//

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> specular_pmrem_shader_resource_view;

	// �J�����̈ʒu
	DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, 10.0f, 1.0f };
	// �}�e���A���J���[
	DirectX::XMFLOAT4 material_color{ 1 ,1, 1, 1 };
	DirectX::XMFLOAT4 Particle_material_color{ 1 ,1, 1, 1 };
	DirectX::XMFLOAT4X4 inverse_view_projection;
	// ���C�g�̕���
	DirectX::XMFLOAT4 light_direction{ 0.0f, -1.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT4 Trans_direction{ 0.0f, 0.0f, -1.0f, 0.0f };
	DirectX::XMFLOAT4 Focus_direction{ 0.0f, 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 Up_direction{ 0.0f, 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 translation{ 0, 0, 0 };
	DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
	DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
	
	float neck = 1.5f;
	float neckLength = 300.0f;
	float animationTimer = 0.5f;
	int animationTimer1[2] = { 0,40 };
	int animationTimer2[2] = { 40,80 };
	float  bloom_intensity;
	float  timeData = 0;
	bool stage1=false;
	bool stage2=false;
	bool scene = true;
	
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];
	// PARTICLE
	bool integrate_particles{ true };
	std::unique_ptr<particles> particles;
	char* er = "";
	int particle_set = 0;
	int particle_bool = 0;
	bool t = true;
	// �����ւ��悤�̃s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader> replaced_pixel_shader;
	DirectX::XMFLOAT3 eyePosition;
	// �����ւ��悤�̃e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> replaced_shader_resource_view;
	float camera_exper = 590.0f;
	float timer{ 0.0f };
	bool flag{ false };
	DirectX::XMFLOAT2 cursor_position;
private:

	 int cameraMode = static_cast<int>(CameraController::Mode::FreeCamera);

	CameraController* cameraconmtroller = nullptr;
	Player* player = nullptr;
	StageMoveFloor* stageMoveFloor[4];
	std::unique_ptr < SwitchFloorButton> switchFloor[3];
	void Render3D(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context);
	void Render2D(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context);
	void ActivateMechanism();
	//cameraEventController
	void EventSet(float elapsed_Time);

};
