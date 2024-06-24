#pragma

#include <DirectXMath.h>
#include <functional>

//�J�����R���g���[���[
class CameraController {

public:
	CameraController() {}
	~CameraController() {}

	//�X�V����
	void Update(float elapsedTime);

	//�^�[�Q�b�g�ʒu����
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	DirectX::XMFLOAT3& GetTarget() { return target; }

	void SetGoal(const DirectX::XMFLOAT3& target,const DirectX::XMFLOAT3& goal, float speed, std::function<void()> callback, bool keepLookingAtPlayer, DirectX::XMFLOAT3& start);
	void MoveTowardsGoal(float elapsedTime);
	DirectX::XMFLOAT3 CalculateCircularPosition(float elapsedTime);
	void ReturnToFreeCameraAfterDelay(float delay);
	void system();
	void MoveInCircularPath(float elapsedTime); // �~�`�Ɉړ�����֐����Ăяo��

	// ���[�h
	enum class  Mode
	{
		FreeCamera,		// �t���[�J����
		GameCamera,	// �C�x���g�J����
		DebugCamara,
	};

	void ChangeCamera(Mode mode);

	void CameraControllerDebugImGui();
	int cameraMode = static_cast<int>(CameraController::Mode::FreeCamera);
private:
	void UpdateTransform();

private:
	float distance;
	DirectX::XMFLOAT3  target = { 0,0,0 };
	DirectX::XMFLOAT3  angle = { 0,0,0 };
	float              rollSpeed = DirectX::XMConvertToRadians(90);
	float              range = 9.0f;
	float zoomSpeed=1.0f; // �ǉ��F�Y�[�����x
	float maxAngleX = DirectX::XMConvertToRadians(45);
	float minAngleX = DirectX::XMConvertToRadians(-45);
	float time;
	
	bool camerabool = false;
	bool camerabool1 = false;
	// �S�[���ւ̈ړ��Ɋւ���ϐ�
	DirectX::XMFLOAT3 StartPosition = { 0,0,0 };
	DirectX::XMFLOAT3 goalPosition = { 0,0,0 };
	DirectX::XMFLOAT3 camera_focus;
	DirectX::XMFLOAT4 camera_position;
	bool moveToGoal=true;
	float moveSpeed=0;
	float goalTolerance=0.5f;
	std::function<void()> onGoalReached;
	DirectX::XMFLOAT4 targeteye;
	bool lookAtPlayer=false;
	// �^�C�}�[�ǉ�
	float freeCameraTimer = 0.0f;
	bool shouldReturnToFreeCamera = false;

public:
	Mode				mode = Mode::FreeCamera;
	

};
























