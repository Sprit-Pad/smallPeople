  #include "CameraController.h"
  #include "Camera.h"
#include "../Input/Input.h"
#include "../imgui/imgui.h"
#include "../SwitchManager.h"
#include "../Global.h"
void CameraController::Update(float elapsedTime)
{
	Camera& camera = Camera::Instance();
	
	Mouse& MousePad = Input::Instance().GetMouse();
	// Get the screen center
	POINT screenCenter;
	screenCenter.x = GetSystemMetrics(SM_CXSCREEN) / 2;
	screenCenter.y = GetSystemMetrics(SM_CYSCREEN) / 2;

	// Get current mouse position
	POINT currentMousePos;
	GetCursorPos(&currentMousePos);

	// Calculate the mouse movement
	float moveX = (currentMousePos.x - screenCenter.x) * 0.04f;
	float moveY = (currentMousePos.y - screenCenter.y) * 0.04f;

	// Reset mouse position to the center of the screen
	SetCursorPos(screenCenter.x, screenCenter.y);

	//カメラの回転速度
	float speed = rollSpeed * elapsedTime;

	
	switch (mode)
	{
	case Mode::FreeCamera :

		
		// マウス操作
		{
			//if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
			{
				// Y軸回転
				angle.y += moveX * 0.1f;
				if (angle.y > DirectX::XM_PI)
					angle.y -= DirectX::XM_2PI;
				else if (angle.y < -DirectX::XM_PI)
					angle.y += DirectX::XM_2PI;
				// X軸回転
				angle.x += moveY * 0.1f;
				if (angle.x > DirectX::XMConvertToRadians(89.9f))
					angle.x = DirectX::XMConvertToRadians(89.9f);
				else if (angle.x < -DirectX::XMConvertToRadians(89.9f))
					angle.x = -DirectX::XMConvertToRadians(89.9f);
			}
	    }
		cameraUpdatebool = true;
	
		break;

	case Mode::GameCamera:
			MoveTowardsGoal(elapsedTime);
			cameraUpdatebool = false;
		//MoveInCircularPath(elapsedTime); // 円形に移動する関数を呼び出す
		break;

	case Mode::DebugCamara:
		// 視線行列を生成
		DirectX::XMMATRIX V;
		// マウス操作
	{
		if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			// Y軸回転
			angle.y += moveX * 0.1f;
			if (angle.y > DirectX::XM_PI)
				angle.y -= DirectX::XM_2PI;
			else if (angle.y < -DirectX::XM_PI)
				angle.y += DirectX::XM_2PI;
			// X軸回転
			angle.x += moveY * 0.1f;
			if (angle.x > DirectX::XMConvertToRadians(89.9f))
				angle.x = DirectX::XMConvertToRadians(89.9f);
			else if (angle.x < -DirectX::XMConvertToRadians(89.9f))
				angle.x = -DirectX::XMConvertToRadians(89.9f);
		}
		else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
		{
			V = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat4(&camera_position),
				DirectX::XMLoadFloat3(&camera_focus),
				DirectX::XMLoadFloat3(&camera.up));
			DirectX::XMFLOAT4X4 W;
			DirectX::XMStoreFloat4x4(&W, DirectX::XMMatrixInverse(nullptr, V));
			// 平行移動
			float s = distance * 0.035f;
			float x = moveX * s;
			float y = moveY * s;
			camera_focus.x -= W._11 * x;
			camera_focus.y -= W._12 * x;
			camera_focus.z -= W._13 * x;
			
			camera_focus.x += W._31 * y;
			camera_focus.y += W._32 * y;
			camera_focus.z += W._33 * y;
		}
		if (MousePad.GetWheel() != 0)	// ズーム
		{
			distance -= static_cast<float>(MousePad.GetWheel()) * distance * 0.001f;
		}
		
	}
		break;
	}
	SwitchManager& switchManager = SwitchManager::Instance();
	time = elapsedTime;
		UpdateTransform();
		
			system();
		
		// タイマー処理
		if (shouldReturnToFreeCamera) {
			freeCameraTimer -= elapsedTime;
			if (freeCameraTimer <= 0.0f) {
				cameraMode = static_cast<int>(CameraController::Mode::FreeCamera);
				
				ChangeCamera(static_cast<CameraController::Mode>(cameraMode));
				cameraUpdatebool = true;
				camerabool = false;
				shouldReturnToFreeCamera = false;
				if (switchManager.GetStage(2)->activeSwitch1) {
					switchManager.GetStage(2)->EventSwitch = true;
				}
			}
		}
}

void CameraController::ChangeCamera(Mode mode)
{
	this->mode = mode;
}

void CameraController::CameraControllerDebugImGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiSetCond_Once);
	ImGui::Begin("CameraController");
	ImGui::RadioButton("Free Camera", &cameraMode, static_cast<int>(CameraController::Mode::FreeCamera));
	ImGui::RadioButton("Game Camera", &cameraMode, static_cast<int>(CameraController::Mode::GameCamera));
	ImGui::RadioButton("Debug Camera", &cameraMode, static_cast<int>(CameraController::Mode::DebugCamara));
	ImGui::DragFloat("range", &range);
	ChangeCamera(static_cast<CameraController::Mode>(cameraMode));
	ImGui::Separator();

	ImGui::End();

}

void CameraController::UpdateTransform()
{
	//カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	

	//回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	//注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT4 eye;
	if(mode==Mode::FreeCamera){
		eye.x = target.x - front.x * range;
		eye.y = target.y - front.y * range;
		eye.z = target.z - front.z * range;
		eye.w = 1.0f;
	}
	
	//カメラの視点と注視点を設定
	if (mode == Mode::FreeCamera) {
		Camera::Instance().SetLookAt(eye, target, { 0, 1, 0 });
	}
	else if (mode == Mode::GameCamera) {
		Camera::Instance().SetLookAt(targeteye, target, { 0, 1, 0 });
	}
	else if (mode == Mode::DebugCamara) {
		float sx = ::sinf(angle.x), cx = ::cosf(angle.x);
		float sy = ::sinf(angle.y), cy = ::cosf(angle.y);
		DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&Camera::Instance().front);
		DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR Distance = DirectX::XMVectorSet(distance, distance, distance, 0.0f);
		Front = DirectX::XMVectorMultiply(Front, Distance);
		DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, Front);
		DirectX::XMStoreFloat4(&camera_position, Eye);
		Camera::Instance().SetLookAt(camera_position, camera_focus, { 0, 1, 0 });
	}
	
}
void CameraController::SetGoal(const DirectX::XMFLOAT3& target,const DirectX::XMFLOAT3& goal, float speed, std::function<void()> callback, bool keepLookingAtPlayer, DirectX::XMFLOAT3& start) {
	StartPosition = target;
	goalPosition = goal;
	moveSpeed = speed;
	moveToGoal = true;
	onGoalReached = callback;
	lookAtPlayer = keepLookingAtPlayer;
	this->target = start;
}
void CameraController::MoveInCircularPath(float elapsedTime)
{
	if (!moveToGoal) return;

	// 円形パスの半径と角速度
	float radius = 5.0f;
	float angularSpeed = DirectX::XMConvertToRadians(45.0f); // 1秒間に45度進む

	// 現在の円周上の角度を計算
	static float angle = 0.0f;
	angle += angularSpeed * elapsedTime;

	// 円形パス上の位置を計算
	DirectX::XMFLOAT3 circularPosition;
	circularPosition.x = radius * cos(angle);
	circularPosition.y = 0.0f; // Y軸は0に固定
	circularPosition.z = radius * sin(angle);

	// カメラの移動速度
	float moveStep = moveSpeed * elapsedTime;

	// 新しい位置を計算
	DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR circularPos = DirectX::XMLoadFloat3(&circularPosition);
	DirectX::XMVECTOR moveVector = DirectX::XMVectorScale(circularPos, moveStep);

	// カメラ位置を更新
	DirectX::XMVECTOR newEyePos = DirectX::XMVectorAdd(targetPos, moveVector);
	DirectX::XMFLOAT4 newEye;
	DirectX::XMStoreFloat4(&newEye, newEyePos);
	Camera::Instance().SetEye(newEye);

	// ゴールに到達したかをチェック
	float distanceToGoal = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(targetPos, newEyePos)));
	if (distanceToGoal < goalTolerance)
	{
		moveToGoal = false;
		if (onGoalReached)
		{
			onGoalReached();
			
		}
	}

	// カメラの挙動を更新
	UpdateTransform();
}
void CameraController::MoveTowardsGoal(float elapsedTime) {
	if (!moveToGoal) return;

	// カメラの現在位置と目標位置の間のベクトルを計算
	DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat4(&Camera::Instance().GetEye());
	//DirectX::XMVECTOR currentPos = DirectX::XMLoadFloat3(&StartPosition);
	DirectX::XMVECTOR goalPos = DirectX::XMLoadFloat3(&goalPosition);
	DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(goalPos, currentPos);
	direction = DirectX::XMVector3Normalize(direction);

	// 移動量を計算
	float moveStep = moveSpeed * elapsedTime;
	DirectX::XMVECTOR moveVector = DirectX::XMVectorScale(direction,moveStep );

	// 新しい位置を計算
	currentPos = DirectX::XMVectorAdd(currentPos, moveVector);

	// カメラ位置を更新
	DirectX::XMFLOAT4 newEye;
	DirectX::XMStoreFloat4(&newEye, currentPos);
	Camera::Instance().SetEye(newEye);
	targeteye = newEye;

	// ゴールに到達したかをチェック
	DirectX::XMFLOAT3 eyePos = { newEye.x, newEye.y, newEye.z };
	DirectX::XMFLOAT3 diff;
	DirectX::XMStoreFloat3(&diff, DirectX::XMVectorSubtract(goalPos, currentPos));
	float distanceSquared = DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(&diff)).m128_f32[0];
	if (distanceSquared < goalTolerance * goalTolerance) {
		moveToGoal = false;
		if (onGoalReached) {
			onGoalReached();
			ReturnToFreeCameraAfterDelay(2.0f);  // 2秒後にFreeCameraに戻るように設定
		}
	}
	// カメラのターゲットを設定
	
    {
		DirectX::XMStoreFloat3(&target, DirectX::XMLoadFloat3(&StartPosition));
	}

	
}
DirectX::XMFLOAT3 CameraController::CalculateCircularPosition(float elapsedTime)
{
	// 円形パスの半径と角速度
	float radius = 5.0f;
	float angularSpeed = DirectX::XMConvertToRadians(45.0f); // 1秒間に45度進む

	// 現在の円周上の角度を計算
	static float angle = 0.0f;
	angle += angularSpeed * elapsedTime;

	// 円形パス上の位置を計算
	DirectX::XMFLOAT3 circularPosition;
	circularPosition.x = radius * cos(angle);
	circularPosition.y = 0.0f; // Y軸は0に固定
	circularPosition.z = radius * sin(angle);

	return circularPosition;
}
void CameraController::ReturnToFreeCameraAfterDelay(float delay) {
	freeCameraTimer = delay;
	shouldReturnToFreeCamera = true;
}

void CameraController::system() 
{
	//一個目のスイッチ
	SwitchManager& switchManager = SwitchManager::Instance();
	
		if (switchManager.GetStage(2)->GetModelType() == ModelSwitchType::TrapOn && !switchManager.GetStage(2)->activeSwitch1) {
			cameraMode = static_cast<int>(CameraController::Mode::GameCamera);
			ChangeCamera(static_cast<CameraController::Mode>(cameraMode));
			//cameraconmtroller->mode = CameraController::Mode::GameCamera;
			//cameraconmtroller->ChangeCamera(cameraconmtroller->mode);
			switchManager.GetStage(2)->activeSwitch1 = true;
			camerabool = true;

		}
	
	
		if (switchManager.GetStage(1)->GetModelType() == ModelSwitchType::TrapOn && !switchManager.GetStage(1)->activeSwitch1) {
			cameraMode = static_cast<int>(CameraController::Mode::GameCamera);
			ChangeCamera(static_cast<CameraController::Mode>(cameraMode));
			//cameraconmtroller->mode = CameraController::Mode::GameCamera;
			//cameraconmtroller->ChangeCamera(cameraconmtroller->mode);
			switchManager.GetStage(1)->activeSwitch1 = true;
			camerabool1 = true;
		}
	

}