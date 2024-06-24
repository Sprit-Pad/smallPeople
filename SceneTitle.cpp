#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Camera/Camera.h"
#include "ShaderContext.h"

//初期化
void SceneTitle::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	
	
	create_ps_from_cso(graphics.GetDevice(), "luminance_extraction_ps.cso", pixel_shaders[0].GetAddressOf());
	create_ps_from_cso(graphics.GetDevice(), "blur_ps.cso", pixel_shaders[1].GetAddressOf());
	graphics.createConstantBuffers<Parametric_constants>(constant_buffers[1].GetAddressOf());
	graphics.createConstantBuffers<ShaderContext::scene_constants>(constant_buffers[0].GetAddressOf());
	framebuffers[0] = std::make_unique<framebuffer>(graphics.GetDevice(), 1280, 720);
	framebuffers[1] = std::make_unique<framebuffer>(graphics.GetDevice(), 1280 / 2, 720 / 2);
	depth_stencil_view = graphics.GetDepthStencilView();
	render_target_view = graphics.GetRenderTargetView();
	bit_block_transfer = std::make_unique<fullscreen_quad>(graphics.GetDevice());
	skinned_meshes[1] = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\nico.fbx", true, 0);
	
	
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
	sprite_batches[0] = std::make_unique<sprite_batch>(graphics.GetDevice(), L".\\resources\\アクションゲーム.png", 1);
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT4(camera_position.x, camera_position.y, camera_position.z,1.0f),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)


	);

	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(30), graphics.GetScreenWidth() / graphics.GetScreenHeight(), 0.1f, 1000.0f);

	eyePosition = { camera_position.x, camera_position.y, camera_position.z };
	// TODO HUSK.02
	huskparticles = std::make_unique<husk_particles>(graphics.GetDevice());
	
}
//終了化
void SceneTitle::Finalize()
{

}
//更新処理
void SceneTitle::Update(float elapsedTime)
{
	Camera& camera = Camera::Instance();
	Graphics& graphics = Graphics::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	immediate_context1 = graphics.GetDeviceContext();
	//何かボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton=
		 GamePad::BTN_A|
		 GamePad::BTN_B|
		 GamePad::BTN_X|
		 GamePad::BTN_Y;
	if (gamePad.GetButtonDown()&anyButton) 
	{
		
		SceneManager::Instance().ChangeScene(std::make_unique<SceneLoading>(std::make_unique<SceneGame>()));
	
	}

	// TODO HUSK.03
	if (GetAsyncKeyState(' ') & 0x01)
	{
		integrate_particles = !integrate_particles;
	}
	if (integrate_particles)
	{
		huskparticles->integrate(immediate_context1.Get(), elapsedTime);
	}

	timeData = elapsedTime;

}
//描画処理
void SceneTitle::Render()
{
    Camera& camera = Camera::Instance();
	Graphics& graphics = Graphics::Instance();
	// 様々な描画命令を GPU に伝える
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context1;
	immediate_context1 = graphics.GetDeviceContext();
	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	immediate_context1->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediate_context1->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	immediate_context1->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

	
	
	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	// ビューポートを取得
	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	immediate_context1->RSGetViewports(&num_viewports, &viewport);

	// アスペクト比を計算
	float aspect_ratio{ viewport.Width / viewport.Height };
	// 左手座標系投資投影でプロジェクション行列を作成
	DirectX::XMMATRIX P{
	DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f)
	};
	{
		// カメラの位置を設定
		DirectX::XMVECTOR eye{ DirectX::XMVectorSet(camera.GetEye().x, camera.GetEye().y, camera.GetEye().z, camera.GetEye().w) };
		//DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&camera_position) };
		// カメラが見るターゲットの位置を設定
		DirectX::XMVECTOR focus{ DirectX::XMVectorSet(camera.GetFront().x, camera.GetFront().y,camera.GetFront().z, 1.0f) };
		// カメラの上の方向を設定
		DirectX::XMVECTOR up{ DirectX::XMVectorSet(camera.GetUp().x,camera.GetUp().y, camera.GetUp().z, 0.0f) };
		// 左手座標系でビュー行列を作成
		DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
		ShaderContext::scene_constants data{};
		////PixelShaderConstants pixsel{};
		//	   // ビュー行列とプロジェクション行列を掛け合わせる(乗算)
		//XMStoreFloat4x4(&data.view_projection, V * P);
		camera.MixViewProjection(data, V, P);
		//// 光の当たる方向を設定
		data.light_direction = { light_direction.x, light_direction.y, light_direction.z, 0 };
		////pixsel.bloomIntensity = bloom_intensity;
		//data.light_direction = light_direction;
		camera_position = camera.GetEye();
		data.camera_position = camera_position;
		//data.view_projection = *camera.GetProjection();
	immediate_context1->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
	}




	/*camera.SetLookAt(DirectX::XMFLOAT4(0.0f, 0.0f, 10.0f, 1.0f),
		DirectX::XMFLOAT4(0, 0, 0, 1),
		DirectX::XMFLOAT4(0, 1, 0, 1));*/
	// ビュー・プロジェクション行列と光の当たる方向をシーン定数バッファとして設定
	//bimmediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &pixsel, 0, 0);

		   // 設定したシーン定数バッファを GPU 側に登録
	immediate_context1->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	//
	immediate_context1->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	// ポストエフェクト用定数バッファに値を設定し、ピクセルシェーダー側に登録
	immediate_context1->UpdateSubresource(constant_buffers[1].Get(), 0, 0, &parametric_constants, 0, 0);
	immediate_context1->PSSetConstantBuffers(2, 1, constant_buffers[1].GetAddressOf());

	// 深度テスト：オン、深度ライト：オンの深度ステンシルステートを設定しておく
//immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
	immediate_context1->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context1->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context1->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	immediate_context1->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	graphics.SetSamplar();
	// 2D 用にラスタライザステートの設定を GPU 側に指定
	immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	{
		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 0);

		// 2D 用にラスタライザステートの設定を GPU 側に指定
		immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_ON)].Get(), 0);

	}
	framebuffers[0]->clear(immediate_context1.Get());
	framebuffers[0]->activate(immediate_context1.Get());
	{

		immediate_context1->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::NONE)].Get(), nullptr, 0xFFFFFFFF);

		// 2D 用にラスタライザステートの設定を GPU 側に指定
		immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);

		////// スプライトバッチの開始
		sprite_batches[0]->begin(immediate_context1.Get(), replaced_pixel_shader.Get(), replaced_shader_resource_view.Get());

		//////// スプライト取り付け
		sprite_batches[0]->render(immediate_context1.Get(),
			0, 0, 1280, 720);

		//// 取り付けたスプライトの一括表示
		sprite_batches[0]->end(immediate_context1.Get());

	}


	{

		immediate_context1->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
		immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);



		scaling = { 0.01f,0.01f,0.01f };
		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scaling.x, scaling.y, scaling.z) };
		DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
		DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(translation.x , translation.y, translation.z) };
		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, S * R * T);

#if 1
		int clip_index{ 0 };
		int frame_index{ 0 };
		static float animation_tick{ 0 };
		clip_index = animation_index;
		animation& animation{ skinned_meshes[1]->animation_clips.at(clip_index) };
		frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
		if (frame_index > animation.sequence.size() - 1)
		{
			frame_index = 0;
			animation_tick = 0;
		}
		else
		{
			animation_tick += timeData;
		}
		animation::keyframe& keyframe{ animation.sequence.at(frame_index) };
#else



#endif
	



	// TODO HUSK.05
	if (!integrate_particles)
	{
		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
		immediate_context1->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::NONE)].Get(), nullptr, 0xFFFFFFFF);
		immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

		DirectX::XMStoreFloat4x4(&world, DirectX::XMLoadFloat4x4(&world) * DirectX::XMMatrixTranslation(0, -1, 0));
		huskparticles->accumulate_husk_particles(immediate_context1.Get(), [&](ID3D11PixelShader* accumulate_husk_particles_ps) {
			skinned_meshes[1]->render(immediate_context1.Get(), world, material_color, &keyframe, accumulate_husk_particles_ps);
			});
	}

	}
	{
		// TODO HUSK.06
		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
		immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context1->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
		immediate_context1->GSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
		huskparticles->render(immediate_context1.Get());
	}
	//framebuffers[0]->deactivate(immediate_context.Get());
	framebuffers[0]->deactivate(immediate_context1.Get());
	framebuffers[1]->clear(immediate_context1.Get());
	framebuffers[1]->activate(immediate_context1.Get());
	immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	// オフスクリーン描画したテクスチャに高輝度抽出用ピクセルシェーダーを設定してオフスクリーン描画
	bit_block_transfer->blit(immediate_context1.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
	framebuffers[1]->deactivate(immediate_context1.Get());

#if 0

	immediate_context1->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
	immediate_context1->RSSetState(rasterizer_states[2].Get());
	// 高輝度抽出したオフスクリーン描画のテクスチャをスクリーンに描画
	bit_block_transfer->blit(immediate_context1.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif
	immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	ID3D11ShaderResourceView* shader_resource_views[2]{ framebuffers[0]->shader_resource_views[0].Get(), framebuffers[1]->shader_resource_views[0].Get() };
	// 高輝度処理をほどこしたテクスチャとブラーをほどこしたテクスチャを合成して描画
	bit_block_transfer->blit(immediate_context1.Get(), shader_resource_views, 0, 2, pixel_shaders[1].Get());

}
