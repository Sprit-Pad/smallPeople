#include "Player.h"
#include "Graphics.h"
#include "Input/Input.h"
#include "Camera/Camera.h"
#include "StageManager.h"
#include "imgui/imgui.h"
#include "SwitchFloorButton.h"
#include "SwitchManager.h"
#include "Global.h"
static Player* instance = nullptr;


Player::Player()
{
	Graphics& graphics = Graphics::Instance();
	scale = { 0.013f,0.013f,0.013f };
	//scale = { 0.02f,0.02f,0.02f };
	skinned_meshes[1] = std::make_unique<skinned_mesh>(graphics.GetDevice(), ".\\resources\\nico.fbx", true, 0,1);
	position.x = -5.897f;
	position.y = -83.111f;
	position.z = -35.929f;
	//position = { -1.023f, -83.111f, 22.361f };
}

Player::~Player()
{

}

void Player::Update(float elapsedTime)
{

	
	
	Camera& camera = Camera::Instance();
	if (cameraUpdatebool) {
		switch (state)
		{
		case State::Idle:

			UpdateIdleState(elapsedTime);

			break;

		case State::Move:

			UpdateMoveState(elapsedTime);

			break;

		case State::Attack:

			UpdateAttackState(elapsedTime);
			break;
		}

		UpdateVelocity(elapsedTime);
		Graphics& graphics = Graphics::Instance();
	}
	
	animationTime = elapsedTime;
	//skinned_meshes[1]->UpdateTransform(transform);
	
	UpdateTransform();
	//skinned_meshes[1]->UpdateTransform(transform);
}
void Player::DebugIMGUI() 
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);


	ImGui::Begin("Player");
	ImGui::InputFloat(u8"処理時間", &averageTime, 0, 0, "%.7f", ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat3("position", &position.x);
	ImGui::InputFloat3("angle", &angle.x);
	ImGui::InputFloat3("scale", &scale.x);
	ImGui::Separator();

	ImGui::End();
}
void Player::Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
	, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[], DirectX::XMFLOAT4 material_color)
{
	Graphics& graphics = Graphics::Instance();

	{

		immediate_context->OMSetBlendState(blend_states[1].Get(), nullptr, 0xFFFFFFFF);
		immediate_context->RSSetState(rasterizer_states[2].Get());
		immediate_context->OMSetDepthStencilState(depth_stencil_states[0].Get(), 0);

			if (blend)
			{
				skinned_meshes[1]->update_animation(keyframe2);
				skinned_meshes[1]->render(immediate_context.Get(), transform, material_color, &keyframe2);

			}
			if (!blend) 
			{
				skinned_meshes[1]->update_animation(keyframe1);
				skinned_meshes[1]->render(immediate_context.Get(), transform, material_color, &keyframe1);

			}
	}
	

}

void Player::UpdateAnimation(float elapsedTime,  int clip_index,float animationBlendTime,float animationBlendSeconds)
{
	
	float blendRate = 1.0f;
	if (AnimationBlendTime < animationBlendSeconds)
	{
		AnimationBlendTime += elapsedTime;
		if (AnimationBlendTime >= animationBlendSeconds)
		{
			AnimationBlendTime = animationBlendSeconds;
		}
		blendRate = AnimationBlendTime / animationBlendSeconds;
		blendRate *= blendRate;
	}
		
	if (blendRate < 1.0f)
	{
		
		
		// ブレンド再生
		animation::keyframe outkeyframe1;
		
		const animation::keyframe* keyframes[2]{
			&keyframe1,
			// 今回アニメーションの最初のフレームを最後として補完
			& skinned_meshes[1]->animation_clips.at(clip_index).sequence.at(0)
		};

		// ブレンド補間
		skinned_meshes[1]->blend_animations(keyframes, blendRate, outkeyframe1);
		keyframe2 = outkeyframe1;
		// 補完したキーフレームを更新
		skinned_meshes[1]->update_animation(keyframe2);
		
		blend = true;
		
		
	}
	else
	{
		// 通常再生
        int frame_index{ 0 };
		static float animation_tick{ 0 };
		

		animation & animation={ skinned_meshes[1]->animation_clips.at(clip_index) };
		frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
		if (frame_index > animation.sequence.size() - 1)
		{
			frame_index = 0;
			animation_tick = 0;
			SetanimationEnd(true);
			animationcurrentTimer = 0;
		}
		else
		{
			animation_tick += elapsedTime;
			animationcurrentTimer = animation_tick;
		}
		keyframe1 = animation.sequence.at(frame_index);
		skinned_meshes[1]->update_animation(keyframe1);
		blend = false;
	}
		
	
}
//AnimationState Player::blendAnimation(float elapsed_time, float animationBlendSeconds,
//	float animation_tick, int animation_index,
//	animation::keyframe& keyframe,
//	animation::keyframe& outkeyframe)
//{
//	float blendRate = 1.0f;
//	if (AnimationBlendTime < animationBlendSeconds)
//	{
//		AnimationBlendTime += elapsed_time;
//		if (AnimationBlendTime >= animationBlendSeconds)
//		{
//			AnimationBlendTime = animationBlendSeconds;
//		}
//		blendRate = AnimationBlendTime / animationBlendSeconds;
//		blendRate *= blendRate;
//	}
//
//	if (blendRate < 1.0f)
//	{
//		// ブレンド再生
//		const animation::keyframe* keyframes[2]{
//			&keyframe,
//			&skinned_meshes[1]->animation_clips.at(animation_index).sequence.at(0),
//		};
//
//		// ブレンド補間
//		skinned_meshes[1]->blend_animations(keyframes, blendRate, outkeyframe);
//		return AnimationState::Blend;
//	}
//	else
//	{
//		// 通常再生
//		int clip_index = animation_index;
//		int frame_index = 0;
//		animation& animation = skinned_meshes[1]->animation_clips.at(clip_index);
//		frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
//		if (frame_index > animation.sequence.size() - 1)
//		{
//			frame_index = 0;
//			animation_tick = 0;
//		}
//		else
//		{
//			animation_tick += elapsed_time;
//		}
//		keyframe = animation.sequence.at(frame_index);
//		return AnimationState::Normal;
//	}
//}
Player& Player::Instance()
{
	// TODO: return ステートメントをここに挿入します
	return *instance;


}

void Player::OnLanding()
{
	if (velocity.y < gravity * 5.0f)
	{
		/*
		TransitionLandState();*/
		if (state != State::Damage && state != State::Death)
		{

			//TransitionLandState();

		}
	}


	jumpCount = 0;
}

void Player::OnDamaged()
{
}

void Player::OnDead()
{
}


//待機ステートへ遷移
void Player::TransitionIdleState()
{

	state = State::Idle;

	//待機アニメーション再生
	SetanimationBlendSeconds(0.2f);
	SetAnimationBlendTime(0);
	animation_index = 0;

}

void Player::UpdateLandState(float elapsedTime)
{
	//velocity.x = velocity.z = 0;
	//if (!model->IsPlayAnimation())
	//{
	//	/*if(IsGround())
	//	{*/

	//	TransitionIdleState();


	//	//}
	//}

}

void Player::UpdateIdleState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (InputMove(elapsedTime))
	{

		TransitionMoveState();
	};
	/*if (InputJump())
	{
		TransitionJumpState();
	};*/
	if (InputAttackState())
	{
		
		TransitionAttackState();
	};
	UpdateAnimation(elapsedTime, static_cast<int>(State::Idle),AnimationBlendTime, AnimationBlendSeconds);
	
		//UpdateIdleState(elapsedTime);
	

	
}
void Player::TransitionMoveState()
{

	state = State::Move;
	SetanimationBlendSeconds(0.2f);
	SetAnimationBlendTime(0);
	//走りアニメーション再生
	//model->PlayAnimation(Anim_Running, true);
	animation_index = 1;

}

bool Player::InputJump()
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	if (gamePad.GetButtonDown() & GamePad::BTN_A) {


		//jump(jumpSpeed);

		if (jumpCount < jumpLimit) {
			jumpCount++;
			jump(jumpSpeed);
			return true;
			/*model->PlayAnimation(6, true, 0.1f);*/
		}
	}




	return false;
}

void Player::TransitionAttackState()
{
	state = State::Attack;
	SetanimationBlendSeconds(0.0f);
	SetAnimationBlendTime(0);
	//走りアニメーション再生
	//model->PlayAnimation(Anim_Running, true);
	animation_index = 7;
}

void Player::UpdateAttackState(float elapsedTime)
{
		
	SwitchManager& SwMa = SwitchManager::Instance();
	if (!IsPlayAnimation())
	{
		TransitionIdleState();
	}
	else {
		UpdateAnimation(elapsedTime, static_cast<int>(State::Attack), AnimationBlendTime, AnimationBlendSeconds);
		int count = SwMa.GetStagesCount();

		for (int i = 0; i < count; i++)
		{
			DirectX::XMFLOAT3 outpos = {};
			std::unique_ptr<SwitchALL>& Switch = SwMa.GetStage(i);
			if (Switch->GetSwitchType() != ModelSwitch::Yukaswitch)continue;
			if (Collision::IntersectSphereVsIntersectSphere(position,0.7f,Switch->GetPosition(),0.7f, outpos))
			{
				if (SwMa.GetStage(1)) {
					Switch->SetON1(true);
				}
				
			}
		}
	}
}



void Player::TransitionJumpState()
{

	state = State::Jump;
	//model->PlayAnimation(Anim_Jump, false);
}

void Player::UpdateMoveState(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	//移動処理
	if (!InputMove(elapsedTime))
	{

		TransitionIdleState();

	}

	Move(moveVec.x, moveVec.z, moveSpeed*2.0f);
	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	if (InputAttackState())
	{
		TransitionAttackState();
	
	};

	/*if (InputJump())
	{
		TransitionJumpState();
	};*/
	
	UpdateAnimation(elapsedTime*1.5f,  static_cast<int>(State::Move), AnimationBlendTime, AnimationBlendSeconds);
}
bool Player::InputAttackState()
{
	Mouse& gamePad = Input::Instance().GetMouse();
	
	
	if (gamePad.GetButtonDown() &Mouse::BTN_LEFT)
	{
		animationEnd = false;
		return true;
	}
	
	return false;
}
void Player::TransitionLandState()
{
	
	state = State::Land;
	

}

void Player::UpdateJumpState(float elapsedTime)
{
	//InputJump();
	InputMove(elapsedTime);
	if (InputJump())
	{
		
	}
	
	
}



DirectX::XMFLOAT3 Player::GetMoveVec() const
{

	//入力情報を取得
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisLX();
	float ay = gamePad.GetAxisLY();

	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();


	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLenght = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
	if (cameraRightLenght > 0.0f) {
		cameraRightX / cameraRightLenght;
		cameraRightZ / cameraRightLenght;
	}


	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLenght = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
	if (cameraRightLenght > 0.0f) {
		cameraFrontX / cameraFrontLenght;
		cameraFrontZ / cameraFrontLenght;
	}

	DirectX::XMFLOAT3 vec;
	vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
	vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
	vec.y = 0.0f;

	return vec;
}

bool Player::IsPlayAnimation()
{
	if(animationcurrentTimer==0)
	{
		return false;
	}
	return true;
}

bool Player::InputMove(float elapsedTime)
{
	//進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	//移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);
	//旋回処理
	Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);

	return moveVec.x > 0 || moveVec.y > 0 || moveVec.z > 0 || moveVec.x < 0 || moveVec.y < 0 || moveVec.z < 0;
}

