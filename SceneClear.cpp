#include "SceneClear.h"
#include "Graphics.h"
#include "framework.h"
#include "ShaderContext.h"
#include "Camera/Camera.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneTitle.h"
void SceneClear::Initialize()
{
	Camera& camera = Camera::Instance();
	//stage = new Stage();
	Graphics& graphics = Graphics::Instance();

	create_ps_from_cso(graphics.GetDevice(), "luminance_extraction_ps.cso", pixel_shaders[0].GetAddressOf());
	create_ps_from_cso(graphics.GetDevice(), "blur_ps.cso", pixel_shaders[1].GetAddressOf());
	graphics.createConstantBuffers<ShaderContext::scene_constants>(constant_buffers[0].GetAddressOf());
	graphics.createConstantBuffers<Parametric_constants>(constant_buffers[1].GetAddressOf());
	framebuffers[0] = std::make_unique<framebuffer>(graphics.GetDevice(), 1280, 720);
	framebuffers[1] = std::make_unique<framebuffer>(graphics.GetDevice(), 1280 / 2, 720 / 2);
	depth_stencil_view = graphics.GetDepthStencilView();
	render_target_view = graphics.GetRenderTargetView();
	bit_block_transfer = std::make_unique<fullscreen_quad>(graphics.GetDevice());
	sprite_batches[0] = std::make_unique<sprite_batch>(graphics.GetDevice(), L".\\resources\\GameClear.png", 1);
	for (int i = 0; i < 4; i++)
	{
		depth_stencil_states[i] = graphics.GetDepthStencilState(i);
		blend_states[i] = graphics.GetBlendState(i);
		rasterizer_states[i] = graphics.GetRasterizerState(i);
	}
	for (int i = 0; i < 5; i++)
	{
		sampler_states[i] = graphics.GetSamplerState(i);

	}
	// PARTICLE
	particles = std::make_unique<decltype(particles)::element_type>(graphics.GetDevice(), 1000);
	particles->initialize(graphics.GetDeviceContext(), 0, 3, DirectX::XMFLOAT3(0, 0, 0));

	//Camera
	camera.SetLookAt(
		DirectX::XMFLOAT4(camera_position.x, camera_position.y + 5.0f, camera_position.z, 1.0f),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(30), graphics.GetScreenWidth() / graphics.GetScreenHeight(), 1.0f, 10000.0f);
	eyePosition = { camera_position.x, camera_position.y, camera_position.z };
	
	cameraconmtroller = new CameraController();
}

void SceneClear::Finalize()
{
	if (cameraconmtroller != nullptr) {

		delete cameraconmtroller;
		cameraconmtroller = nullptr;

	}
}

void SceneClear::Update(float elapsed_Time)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	
	//何かボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y;
	if (gamePad.GetButtonDown() & anyButton)
	{

		SceneManager::Instance().ChangeScene(std::make_unique<SceneLoading>(std::make_unique<SceneTitle>()));

	}
	// PARTICLE
	
	particles->integrate(graphics.GetDeviceContext(), elapsed_Time, 3, 1, DirectX::XMFLOAT3(0,0,0));
	
	

	camera.CalculateFrustum(camera.frustum, camera.NearP, camera.FarP, camera_exper);
	camera.SetLookAt(camera.eye, camera.front, camera.up);
	//cameraconmtroller->Update(elapsed_Time);

	ShaderContext rc;
	timeData = elapsed_Time;
	
}

void SceneClear::Render()
{
	// フレームバッファはレンダーターゲットビューおよびシェーダーリソースビューとしてシェーダーに
// 同時にバインドすることが出来ない警告の解決
	Graphics& graphics = Graphics::Instance();
	Mouse& MousePad = Input::Instance().GetMouse();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
	immediate_context = graphics.GetDeviceContext();


	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	immediate_context->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediate_context->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	immediate_context->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	// ビューポートを取得
	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);
	//Camera設定
	{
		Camera& camera = Camera::Instance();


		// 左手座標系投資投影でプロジェクション行列を作成
		DirectX::XMMATRIX P{
			DirectX::XMLoadFloat4x4(&camera.GetProjection())
		};

		DirectX::XMMATRIX V{
			DirectX::XMLoadFloat4x4(&camera.GetView())
		};

		ShaderContext::scene_constants data{};
		camera.MixViewProjection(data, V, P);
		DirectX::XMStoreFloat4x4(&data.inverse_view_projection, DirectX::XMMatrixInverse(nullptr, V * P));

		//// 光の当たる方向を設定
		data.light_direction = { light_direction.x, light_direction.y, light_direction.z, 0 };

		camera_position = camera.GetEye();
		data.camera_position = camera_position;

		immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	}
	// 設定したシーン定数バッファを GPU 側に登録
	immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	//
	immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	// ポストエフェクト用定数バッファに値を設定し、ピクセルシェーダー側に登録
	immediate_context->UpdateSubresource(constant_buffers[1].Get(), 0, 0, &parametric_constants, 0, 0);
	immediate_context->PSSetConstantBuffers(2, 1, constant_buffers[1].GetAddressOf());

	// 深度テスト：オン、深度ライト：オンの深度ステンシルステートを設定しておく

	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
	graphics.SetSamplar();
	// 2D 用にラスタライザステートの設定を GPU 側に指定
	immediate_context->RSSetState(rasterizer_states[0].Get());
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[1].Get(), 0);

		// 2D 用にラスタライザステートの設定を GPU 側に指定
		immediate_context->RSSetState(rasterizer_states[0].Get());

		immediate_context->OMSetDepthStencilState(depth_stencil_states[2].Get(), 0);

	}

	framebuffers[0]->clear(immediate_context.Get());
	framebuffers[0]->activate(immediate_context.Get());

	Render2D(immediate_context.Get());
	Render3D(immediate_context.Get());


#if 0
	bit_block_transfer->blit(immediate_context.Get(),
		framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif

	framebuffers[0]->deactivate(immediate_context.Get());
	framebuffers[1]->clear(immediate_context.Get());
	framebuffers[1]->activate(immediate_context.Get());
	immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[2].Get());
	// オフスクリーン描画したテクスチャに高輝度抽出用ピクセルシェーダーを設定してオフスクリーン描画
	bit_block_transfer->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
	framebuffers[1]->deactivate(immediate_context.Get());

#if 0
	immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[2].Get());
	// 高輝度抽出したオフスクリーン描画のテクスチャをスクリーンに描画
	bit_block_transfer->blit(immediate_context.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif
	immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[2].Get());
	ID3D11ShaderResourceView* shader_resource_views[2]{ framebuffers[0]->shader_resource_views[0].Get(), framebuffers[1]->shader_resource_views[0].Get() };
	// 高輝度処理をほどこしたテクスチャとブラーをほどこしたテクスチャを合成して描画
	bit_block_transfer->blit(immediate_context.Get(), shader_resource_views, 0, 2, pixel_shaders[1].Get());
}

void SceneClear::Render3D(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context)
{
	Graphics& graphics = Graphics::Instance();


	// PARTICLE
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetBlendState(blend_states[2].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->GSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
		particles->render(immediate_context.Get());
	}
	
}

void SceneClear::Render2D(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context)
{
	Graphics& graphics = Graphics::Instance();
	
	{
		immediate_context->OMSetBlendState(blend_states[0].Get(), nullptr, 0xFFFFFFFF);
		// 2D 用にラスタライザステートの設定を GPU 側に指定
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
		////// スプライトバッチの開始
		sprite_batches[0]->begin(immediate_context.Get(), replaced_pixel_shader.Get(), replaced_shader_resource_view.Get());
		//////// スプライト取り付け
		sprite_batches[0]->render(immediate_context.Get(),
			0, 0, 1280, 720);
		//// 取り付けたスプライトの一括表示
		sprite_batches[0]->end(immediate_context.Get());
	}
}