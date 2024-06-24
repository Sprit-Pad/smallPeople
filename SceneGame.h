#pragma once


#include "Scene.h"
#include "Player.h"
#include "StageMoveFloor.h"
#include "SwitchMain.h"
#include "SwitchFloorButton.h"

// ゲームシーン
class SceneGame :public Scene
{
public:
	SceneGame() {}
	~SceneGame()override {}

	// 初期化
	void Initialize()override;

	// 終了化
	void Finalize()override;

	// 更新処理
	void Update(float elapsed_Time)override;

	// 描画処理
	void Render()override;
	//ステージ処理
	void EventSystemMovie();

private:

	float ConvertToradians;
	// DirectX11 で利用する様々なリソースを作成する大元
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	// キャンバスに描いた画を額に入れる
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
	// 色を書き込む先のキャンバス
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
	// 奥行情報を書き込むキャンバス
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;
	// サンプラーステート
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[5];
	// 深度ステンシルステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];
	// ブレンドステート(0...無し、1...アルファ、2...加算、3...乗算)
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];
	//フレームバッファ
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
	//	DirectX::XMFLOAT4X4 view_projection; //ビュー・プロジェクション変換行列
	//	DirectX::XMFLOAT4 light_direction; //ライトの向き
	//	// 
	//	DirectX::XMFLOAT4 camera_position;//カメラ位置
	//	//
	//
	//};
	//// ポストエフェクト用定数バッファ
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
	// ポストエフェクト用定数バッファ

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

	// カメラの位置
	DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, 10.0f, 1.0f };
	// マテリアルカラー
	DirectX::XMFLOAT4 material_color{ 1 ,1, 1, 1 };
	DirectX::XMFLOAT4 Particle_material_color{ 1 ,1, 1, 1 };
	DirectX::XMFLOAT4X4 inverse_view_projection;
	// ライトの方向
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
	// 差し替えようのピクセルシェーダ
	Microsoft::WRL::ComPtr<ID3D11PixelShader> replaced_pixel_shader;
	DirectX::XMFLOAT3 eyePosition;
	// 差し替えようのテクスチャ
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
