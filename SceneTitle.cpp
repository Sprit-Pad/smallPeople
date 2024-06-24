#include "SceneTitle.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Camera/Camera.h"
#include "ShaderContext.h"

//������
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
	sprite_batches[0] = std::make_unique<sprite_batch>(graphics.GetDevice(), L".\\resources\\�A�N�V�����Q�[��.png", 1);
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
//�I����
void SceneTitle::Finalize()
{

}
//�X�V����
void SceneTitle::Update(float elapsedTime)
{
	Camera& camera = Camera::Instance();
	Graphics& graphics = Graphics::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>	immediate_context1 = graphics.GetDeviceContext();
	//�����{�^������������Q�[���V�[���֐؂�ւ�
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
//�`�揈��
void SceneTitle::Render()
{
    Camera& camera = Camera::Instance();
	Graphics& graphics = Graphics::Instance();
	// �l�X�ȕ`�施�߂� GPU �ɓ`����
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context1;
	immediate_context1 = graphics.GetDeviceContext();
	ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	immediate_context1->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediate_context1->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	immediate_context1->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);

	
	
	FLOAT color[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	// �r���[�|�[�g���擾
	D3D11_VIEWPORT viewport;
	UINT num_viewports{ 1 };
	immediate_context1->RSGetViewports(&num_viewports, &viewport);

	// �A�X�y�N�g����v�Z
	float aspect_ratio{ viewport.Width / viewport.Height };
	// ������W�n�������e�Ńv���W�F�N�V�����s����쐬
	DirectX::XMMATRIX P{
	DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f)
	};
	{
		// �J�����̈ʒu��ݒ�
		DirectX::XMVECTOR eye{ DirectX::XMVectorSet(camera.GetEye().x, camera.GetEye().y, camera.GetEye().z, camera.GetEye().w) };
		//DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&camera_position) };
		// �J����������^�[�Q�b�g�̈ʒu��ݒ�
		DirectX::XMVECTOR focus{ DirectX::XMVectorSet(camera.GetFront().x, camera.GetFront().y,camera.GetFront().z, 1.0f) };
		// �J�����̏�̕�����ݒ�
		DirectX::XMVECTOR up{ DirectX::XMVectorSet(camera.GetUp().x,camera.GetUp().y, camera.GetUp().z, 0.0f) };
		// ������W�n�Ńr���[�s����쐬
		DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
		ShaderContext::scene_constants data{};
		////PixelShaderConstants pixsel{};
		//	   // �r���[�s��ƃv���W�F�N�V�����s����|�����킹��(��Z)
		//XMStoreFloat4x4(&data.view_projection, V * P);
		camera.MixViewProjection(data, V, P);
		//// ���̓����������ݒ�
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
	// �r���[�E�v���W�F�N�V�����s��ƌ��̓�����������V�[���萔�o�b�t�@�Ƃ��Đݒ�
	//bimmediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &pixsel, 0, 0);

		   // �ݒ肵���V�[���萔�o�b�t�@�� GPU ���ɓo�^
	immediate_context1->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	//
	immediate_context1->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	// �|�X�g�G�t�F�N�g�p�萔�o�b�t�@�ɒl��ݒ肵�A�s�N�Z���V�F�[�_�[���ɓo�^
	immediate_context1->UpdateSubresource(constant_buffers[1].Get(), 0, 0, &parametric_constants, 0, 0);
	immediate_context1->PSSetConstantBuffers(2, 1, constant_buffers[1].GetAddressOf());

	// �[�x�e�X�g�F�I���A�[�x���C�g�F�I���̐[�x�X�e���V���X�e�[�g��ݒ肵�Ă���
//immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);
	immediate_context1->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context1->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context1->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	immediate_context1->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	graphics.SetSamplar();
	// 2D �p�Ƀ��X�^���C�U�X�e�[�g�̐ݒ�� GPU ���Ɏw��
	immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	{
		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 0);

		// 2D �p�Ƀ��X�^���C�U�X�e�[�g�̐ݒ�� GPU ���Ɏw��
		immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_ON)].Get(), 0);

	}
	framebuffers[0]->clear(immediate_context1.Get());
	framebuffers[0]->activate(immediate_context1.Get());
	{

		immediate_context1->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::NONE)].Get(), nullptr, 0xFFFFFFFF);

		// 2D �p�Ƀ��X�^���C�U�X�e�[�g�̐ݒ�� GPU ���Ɏw��
		immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

		immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);

		////// �X�v���C�g�o�b�`�̊J�n
		sprite_batches[0]->begin(immediate_context1.Get(), replaced_pixel_shader.Get(), replaced_shader_resource_view.Get());

		//////// �X�v���C�g���t��
		sprite_batches[0]->render(immediate_context1.Get(),
			0, 0, 1280, 720);

		//// ���t�����X�v���C�g�̈ꊇ�\��
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
	// �I�t�X�N���[���`�悵���e�N�X�`���ɍ��P�x���o�p�s�N�Z���V�F�[�_�[��ݒ肵�ăI�t�X�N���[���`��
	bit_block_transfer->blit(immediate_context1.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
	framebuffers[1]->deactivate(immediate_context1.Get());

#if 0

	immediate_context1->OMSetDepthStencilState(depth_stencil_states[3].Get(), 0);
	immediate_context1->RSSetState(rasterizer_states[2].Get());
	// ���P�x���o�����I�t�X�N���[���`��̃e�N�X�`�����X�N���[���ɕ`��
	bit_block_transfer->blit(immediate_context1.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif
	immediate_context1->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context1->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	ID3D11ShaderResourceView* shader_resource_views[2]{ framebuffers[0]->shader_resource_views[0].Get(), framebuffers[1]->shader_resource_views[0].Get() };
	// ���P�x�������قǂ������e�N�X�`���ƃu���[���قǂ������e�N�X�`�����������ĕ`��
	bit_block_transfer->blit(immediate_context1.Get(), shader_resource_views, 0, 2, pixel_shaders[1].Get());

}
