
#include "SceneGame.h"
#include "Graphics.h"
#include "framework.h"
#include "ShaderContext.h"
#include "Camera/Camera.h"
#include "SceneLoading.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "StageManager.h"
#include "StageMain.h"
#include "SwitchManager.h"
#include "SwitchGoal.h"
#include "SceneClear.h"


// 初期化
void SceneGame::Initialize()
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
	sprite_batches[0] = std::make_unique<sprite_batch>(graphics.GetDevice(), L".\\resources\\screenshot.jpg", 1);
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
	particles->initialize(graphics.GetDeviceContext(), 2, particle_set, DirectX::XMFLOAT3(0, 0, 0));

	//Camera
	camera.SetLookAt(
		DirectX::XMFLOAT4(camera_position.x, camera_position.y + 5.0f, camera_position.z, 1.0f),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(DirectX::XMConvertToRadians(30), graphics.GetScreenWidth() / graphics.GetScreenHeight(), 1.0f, 10000.0f);
	eyePosition = { camera_position.x, camera_position.y, camera_position.z };
	cameraconmtroller = new CameraController();

	player = new Player();
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain();
	stageMain->SetActive(true);
	stageManager.Register(stageMain);
	// キャラクターオブジェクトの作成
	{
		stageMoveFloor[0] = new StageMoveFloor();
		stageMoveFloor[0]->SetStartPoint(DirectX::XMFLOAT3(2.023f, -83.111f, 22.361f));
		stageMoveFloor[0]->SetGoalPoint(DirectX::XMFLOAT3(2.023f, -57.9f, 22.361f));
		stageMoveFloor[0]->SetTorque(DirectX::XMFLOAT3(0, 0.0f, 0));
		stageMoveFloor[0]->SetActive(true); // 最初のステージはアクティブ
		stageManager.Register(stageMoveFloor[0]);
	}
	{
		stageMoveFloor[1] = new StageMoveFloor();
		stageMoveFloor[1]->SetStartPoint(DirectX::XMFLOAT3(10.882f, -70.524f, 14.028f));
		stageMoveFloor[1]->SetGoalPoint(DirectX::XMFLOAT3(9.446f, -74.534f, 20.661f));
		stageMoveFloor[1]->SetTorque(DirectX::XMFLOAT3(0, 0.0f, 0));
		stageMoveFloor[1]->SetActive(false); // 最初のステージはアクティブ
		stageManager.Register(stageMoveFloor[1]);
	}
	{
		stageMoveFloor[2] = new StageMoveFloor();
		stageMoveFloor[2]->SetStartPoint(DirectX::XMFLOAT3(0.128f, -66.487f, 13.096f));
		stageMoveFloor[2]->SetGoalPoint(DirectX::XMFLOAT3(8.023f, -70.524f, 13.096f));
		stageMoveFloor[2]->SetTorque(DirectX::XMFLOAT3(0, 0.0f, 0));
		stageMoveFloor[2]->SetActive(true); // 最初のステージはアクティブ
		stageManager.Register(stageMoveFloor[2]);
	}
	SwitchManager& switchManager = SwitchManager::Instance();
	//スイッチ
	{
		std::unique_ptr<SwitchMain> switches = std::make_unique<SwitchMain>();
		switches->SetActive1(false);
		switchManager.Register(std::move(switches));
	}
	{
		switchFloor[0] = std::make_unique <SwitchFloorButton>();
		switchFloor[0]->SetActive1(true);
		switchFloor[0]->SetPosition(DirectX::XMFLOAT3(8.775f,- 75.143f,23.195f));
		switchFloor[0]->SetId(1);
		switchFloor[0]->SetSwitchCountType(SwitchCountType::Switch1);
		switchManager.Register(std::move(switchFloor[0]));
		
	}
	{
		switchFloor[1] = std::make_unique <SwitchFloorButton>();
		switchFloor[1]->SetActive1(true);
		switchFloor[1]->SetPosition(DirectX::XMFLOAT3(-2.023f, -65.5f, 28.361f));
		switchFloor[1]->SetId(2);
		switchFloor[1]->SetSwitchCountType(SwitchCountType::Switch2);
		switchManager.Register(std::move(switchFloor[1]));
	}
	{
		std::unique_ptr <SwitchGoal>GoalSwitch = std::make_unique<SwitchGoal>();
		GoalSwitch->SetActive1(false);
		GoalSwitch->SetPosition(DirectX::XMFLOAT3(9.048f, -83.111f, -5.237f));
		GoalSwitch->SetId(3);
		GoalSwitch->SetSwitchCountType(SwitchCountType::Switch3);
		switchManager.Register(std::move(GoalSwitch));
	}
	{
		//	空描画用スプライト
		sky_box_render_sprite = std::make_unique<sprite>(graphics.GetDevice(), L".\\resources\\country_club_4k\\diffuse_iem.dds",true);
	}
	{
		cameraconmtroller->SetGoal({ 9.882f, -70.524f, 14.028f },{ -2.882f, -68.524f, 14.028f }, 5.0f, [this]() {
			// 仕掛けが動く処理
			EventSystemMovie();
			}, false, DirectX::XMFLOAT3(10.882f, -70.524f, 14.028f));
	}
	scene = true;
	parametric_constants.exposure = 1.0f;
}

// 終了化
void SceneGame::Finalize()
{

	if (cameraconmtroller != nullptr) {

		delete cameraconmtroller;
		cameraconmtroller = nullptr;

	}
	if (player != nullptr) {

		delete player;
		player = nullptr;

	}
	StageManager::Instance().Clear();
	SwitchManager::Instance().Clear();
}

// 更新処理
void SceneGame::Update(float elapsed_Time)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	//// PARTICLE
	//if (GetAsyncKeyState(' ') & 0x8000)
	//{
	//	t = !t;
	//}
	//if (t)
	//{
	//	particles->initialize(graphics.GetDeviceContext(), 0, particle_set, DirectX::XMFLOAT3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z));
	//	particle_bool = particle_set;
	//	t = false;
	//}
	//if (integrate_particles)
	//{
	//	particles->integrate(graphics.GetDeviceContext(), elapsed_Time, particle_set, particle_bool, player->GetPosition());
	//}
	GamePad& gamePad = Input::Instance().GetGamePad();

	camera.CalculateFrustum(camera.frustum, camera.NearP, camera.FarP, camera_exper);


	//何かボタンを押したらゲームシーンへ切り替え
	const GamePadButton anyButton =
		GamePad::BTN_A |
		GamePad::BTN_B |
		GamePad::BTN_X |
		GamePad::BTN_Y;
	if (gamePad.GetButtonDown() & anyButton)
	{

		SceneManager::Instance().ChangeScene(std::make_unique<SceneLoading>(std::make_unique<SceneClear>()));

	}
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

	ShaderContext rc;
	timeData = elapsed_Time;
	//ImGui::SetNextWindowSize(ImVec2(280, 300), ImGuiSetCond_Once);
	//ImGui::SetNextWindowSize(ImVec2(280, 300), ImGuiSetCond_Once);
	//ImGui::Begin("ImGUI");
	//ImGui::InputInt("particle_set", &particle_set);
	//ImGui::SliderFloat("light_direction.x", &light_direction.x, -1.0f, +1.0f);
	//ImGui::SliderFloat("light_direction.y", &light_direction.y, -1.0f, +1.0f);
	//ImGui::SliderFloat("light_direction.z", &light_direction.z, -1.0f, +1.0f);
	//ImGui::ColorEdit4("material_color", reinterpret_cast<float*>(&material_color));
	//ImGui::ColorEdit4("Particle_material_color", reinterpret_cast<float*>(&Particle_material_color));
	//// ImGuiウィンドウ内でbloom_intensityを設定
	//ImGui::SliderFloat("luminance_extraction_lower_edge", &parametric_constants.luminance_extraction_lower_edge, +0.0f, +1.0f);
	//ImGui::SliderFloat("luminance_extraction_higher_edge", &parametric_constants.luminance_extraction_higher_edge, +0.0f, +5.0f);
	//ImGui::SliderFloat("gaussian_sigma", &parametric_constants.gaussian_sigma, +0.0f, +10.0f);
	//ImGui::SliderFloat("bloom_intensity", &parametric_constants.bloom_intensity, +0.0f, +10.0f);
	//ImGui::SliderFloat("exposure", &parametric_constants.exposure, +0.0f, +10.0f);
	//ImGui::Checkbox("MoveFloor", &stage1);
	//ImGui::Checkbox("MoveFloor", &stage2);
	//camera.DebugImGui();
	//cameraconmtroller->CameraControllerDebugImGui();
	//SwitchManager::Instance().DebugImguiStage();
	if (stage1)
	{
		stageMoveFloor[1]->SetActive(true);

	}
	

	if (stage2)
	{
		SwitchManager::Instance().GetStage(3)->SetActive1(true);

	}
	
	if (scene)
	{
		if (SwitchManager::Instance().GetStage(3)->GetModelType() == ModelSwitchType::TrapOn) {
			scene = false;
		}
    }
	if (!scene) 
	{
		parametric_constants.exposure -= 0.01f;
		if (parametric_constants.exposure <= 0&& !scene)
		{
			scene = true;
			
			SceneManager::Instance().ChangeScene(std::make_unique<SceneLoading>(std::make_unique<SceneClear>()));
			parametric_constants.exposure = 0.0f;
		}
	}
	//player->DebugIMGUI();
	//ImGui::Separator();
	//ImGui::End();

	player->Update(elapsed_Time);

	EventSet(elapsed_Time);
	
	cameraconmtroller->Update(elapsed_Time);

	StageManager::Instance().Update(elapsed_Time);
	SwitchManager::Instance().Update(elapsed_Time);
}

// 描画処理
void SceneGame::Render()
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

	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	graphics.SetSamplar();
	// 2D 用にラスタライザステートの設定を GPU 側に指定
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 0);

		// 2D 用にラスタライザステートの設定を GPU 側に指定
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_ON)].Get(), 0);

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
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	// オフスクリーン描画したテクスチャに高輝度抽出用ピクセルシェーダーを設定してオフスクリーン描画
	bit_block_transfer->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
	framebuffers[1]->deactivate(immediate_context.Get());

#if 0
	immediate_context->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[2].Get());
	// 高輝度抽出したオフスクリーン描画のテクスチャをスクリーンに描画
	bit_block_transfer->blit(immediate_context.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	ID3D11ShaderResourceView* shader_resource_views[2]{ framebuffers[0]->shader_resource_views[0].Get(), framebuffers[1]->shader_resource_views[0].Get() };
	// 高輝度処理をほどこしたテクスチャとブラーをほどこしたテクスチャを合成して描画
	bit_block_transfer->blit(immediate_context.Get(), shader_resource_views, 0, 2, pixel_shaders[1].Get());
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}


void SceneGame::EventSystemMovie() 
{
	float i  = 0;
	stage1 = true;
	
}

void  SceneGame::Render3D(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context)
{
	Graphics& graphics = Graphics::Instance();
	//ステージ
	{
		StageManager::Instance().Render(immediate_context.Get(), rasterizer_states, depth_stencil_states, blend_states, material_color);
	}
	//switch
	{
		SwitchManager::Instance().Render(immediate_context.Get(), rasterizer_states, depth_stencil_states, blend_states, material_color);
	}

	// PARTICLE
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ADD)].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->GSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
		particles->render(immediate_context.Get());
	}
	//プレイヤー
	{
		player->Render(immediate_context.Get(), rasterizer_states, depth_stencil_states, blend_states, material_color);
	}
}

void SceneGame::Render2D(Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context)
{
	Graphics& graphics = Graphics::Instance();
	if (sky_box_render_sprite)
	{
		//	ブレンドステート設定
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
		//	深度ステンシルステート設定
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
		// ラスタライザーステート設定
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		//	シェーダー設定
		immediate_context->IASetInputLayout(sky_box_input_layout.Get());
		immediate_context->VSSetShader(sky_box_vertex_shader.Get(), nullptr, 0);
		immediate_context->PSSetShader(sky_box_pixel_shader.Get(), nullptr, 0);

		//	描画
		sky_box_render_sprite->render(immediate_context.Get(), 0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight());
	}
	{
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ADD)].Get(), nullptr, 0xFFFFFFFF);
		// 2D 用にラスタライザステートの設定を GPU 側に指定
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
		
		//////// スプライトバッチの開始
		//sprite_batches[0]->begin(immediate_context.Get(), replaced_pixel_shader.Get(), replaced_shader_resource_view.Get());
		////////// スプライト取り付け
		//sprite_batches[0]->render(immediate_context.Get(),
		//	0, 0, 1280, 720);
		////// 取り付けたスプライトの一括表示
		//sprite_batches[0]->end(immediate_context.Get());
	}
}

void SceneGame::ActivateMechanism()
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	SwitchManager& switchManager = SwitchManager::Instance();
	stage2 = true;
	switchManager.GetStage(3)->EventSwitch = false;
	scene = true;
}

void SceneGame::EventSet(float elapsed_Time)
{

	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	DirectX::XMFLOAT3 target = player->GetPosition();
	target.y += 1.0f;
	if (cameraconmtroller->cameraMode == static_cast<int>(CameraController::Mode::FreeCamera)) {
		cameraconmtroller->SetTarget(target);

	}
	SwitchManager& switchManager = SwitchManager::Instance();
	if (switchManager.GetStage(2)->activeSwitch1 && switchManager.GetStage(2)->EventSwitch)
	{
		cameraconmtroller->SetGoal({ 9.048f, -83.111f, -5.237f }, { 2.248f, -72.524f, -5.218f }, 5.0f, [this]() {
			// 仕掛けが動く処理
			ActivateMechanism();
			}, false, DirectX::XMFLOAT3(camera.GetEye().x, camera.GetEye().y, camera.GetEye().z));
		switchManager.GetStage(2)->EventSwitch = false;
	}

	

}
