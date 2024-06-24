#include "Character.h"
#include "MathF.h"
#include "StageManager.h"
#include "Camera/Camera.h"
#include "StageMoveFloor.h"
#include "StageMain.h"
//�s��s������
void Character::UpdateTransform() {

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
	DirectX::XMMATRIX R = Y * X * Z;
	DirectX::XMMATRIX W = S * R * T;

	DirectX::XMStoreFloat4x4(&transform, W);

}

void Character::UpdateVelocity(float elapsedTime)
{

	//�o�߃t���[��
	float elapsedFrame = 60.0f * elapsedTime;

	UpdateVerticalVelocity(elapsedFrame);

	UpdateHorizontalVelocity(elapsedFrame);

	UpdateVerticalMove(elapsedTime);

	UpdateHorizontalMove(elapsedTime);

}
bool Character::ApplyDamage(int damage, float invincibleTime)
{
	if (damage == 0)return false;
	if (health <= -1)return false;

	health -= damage;
	if (invincibleTimer > 0.0f)return false;
	invincibleTimer = invincibleTime;
	if (health <= 0) {
		OnDead();
	}
	else {
		OnDamaged();
	}
	return true;
}

void Character::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;

}

void Character::Move(float vx, float vz, float speed)
{
	moveVecX = vx;
	moveVecZ = vz;
	maxMoveSpeed = speed;
}

void Character::jump(float speed)
{
	velocity.y += speed;
}

//���͏����X�V

void Character::Turn(float elaspsedTime, float vx, float vz, float speed)
{
	speed *= elaspsedTime;

	//�i�s�x�N�g�����[���x�N�g���̏ꍇ�͏����Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (fabs(vx) < 0.001f && fabs(vz) < 0.001f) {
		return;
	}

	vx /= length;
	vz /= length;

	//���g�̉�]�l����O���������߂�
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	//��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	//���ϒl
	//
	float rot = 1.0f - dot;
	if (rot > speed) {
		rot = speed;
	}
	//���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontZ * vx) - (frontX * vz);


	if (cross < 0.0f) {
		//�����v
		angle.y -= rot;

	}
	else {
		//���v
		angle.y += rot;
	}




}

void Character::UpdateInvincibleTimer(float elapsedTime)
{

	if (invincibleTimer > 0.0f) {

		invincibleTimer -= elapsedTime;

	}

}

void Character::UpdateVerticalVelocity(float elapsedFrame)
{

	velocity.y += gravity * elapsedFrame;

}

void Character::UpdateVerticalMove(float elapsedTime)
{

	slopeRate = 0.0f;

	//���������̈ړ���
	float my = velocity.y * elapsedTime;

	//������
	if (my < 0.0f)
	{

		////���C�̊J�n�ʒu�͑����̏�����
		//DirectX::XMFLOAT3 start = { position.x,position.y + stepOffset,position.z };
		////���C�̏I���ʒu�͈ړ���̈ʒu
		//DirectX::XMFLOAT3 end = { position.x,position.y + my,position.z };
		// ���C�L���X�g�̐ݒ�
		DirectX::XMFLOAT4 rayStart{ position.x, position.y + stepOffset, position.z, 1.0f };
		DirectX::XMFLOAT4 rayDirection{ 0.0f, -1.0f,0.0f, 1.0f }; // �������̃��C
		DirectX::XMFLOAT4 start = { position.x, position.y , position.z, 1.0f };
		DirectX::XMFLOAT4 end = { position.x, position.y + (my), position.z,1.0f };
		// ���C�L���X�g�̌��ʂ��i�[����ϐ�
		DirectX::XMFLOAT4 hitPoint;
		DirectX::XMFLOAT3 hitNormal;
		DirectX::XMFLOAT3 Normal = { 0,1,0 };
		std::string hitMesh;
		std::string hitMaterial;
		timer.tick();
		
			if (StageManager::Instance().Raycast(rayStart, end, hitPoint, hitNormal, hitMesh, hitMaterial, ModelType::StageMain, e1) )
			{
				
				position.y = hitPoint.y;
				
				Normal = hitNormal;
				float normalLengthXZ = sqrtf(hitNormal.x * hitNormal.x + hitNormal.z * hitNormal.z);
				slopeRate = 1.0f - (hitNormal.y / (normalLengthXZ + hitNormal.y));
				
				if (velocity.y < 0)
				{

					//���n����
					if (!isGround)
					{
						OnLanding();
					}
					isGround = true;
					velocity.y = 0.0f;
				}

			}
			 if (StageManager::Instance().Raycast(rayStart, end, hitPoint, hitNormal, hitMesh, hitMaterial, ModelType::StageMoveFloor, e)) {

				position.x = hitPoint.x;
				position.y = hitPoint.y;
				position.z = hitPoint.z;
				if (velocity.y < 0)
				{

					//���n����
					if (!isGround)
					{
						OnLanding();
					}
					isGround = true;
					velocity.y = 0.0f;
				}
			
			}
			//�㏸��
			else
			{
				//�󒆂ɕ����Ă���
				position.y += my;
				isGround = false;

			}
			timer.tick();
			timerInter += timer.time_interval();
			frames++;
			if (frames == 60)
			{
				averageTime = timerInter / frames;
				frames = 0;
				timerInter = 0;
			}
		
	}
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
	
}
void Character::DebugImgui() 
{
	
}
void Character::UpdateHorizontalVelocity(float elapsedFrame)
{


	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

	if (length > 0.0f) {

		float friction = this->friction * elapsedFrame;

		if (!isGround) { friction *= airControl; }

		if (length > friction) {

			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;

		}
		else {


			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}


	}
	if (length <= maxMoveSpeed) {

		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f) {

			float acceleration = this->acceleration * elapsedFrame;

			if (!isGround) { acceleration *= airControl; }

			velocity.x += moveVecX * acceleration;
			velocity.z += moveVecZ * acceleration;

			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

			if (length > maxMoveSpeed)
			{

				float vx = velocity.x / length;
				float vz = velocity.z / length;

				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;

			}
			if (isGround && slopeRate > 0.0f)
			{

				velocity.y -= length * slopeRate * elapsedFrame;

			}
		}
	}

	moveVecX = 0.0f;
	moveVecZ = 0.0f;

}

void Character::UpdateHorizontalMove(float elapsedTime)
{

	// �����ړ��l
	float mx = velocity.x * elapsedTime;
	float mz = velocity.z * elapsedTime;
	if (!(mx * mx + mz * mz <= 0.0001f))
	{

			//	// ���C�L���X�g�̐ݒ�
		DirectX::XMFLOAT4 rayStart{ position.x, position.y + stepOffset , position.z, 1.0f };
		//DirectX::XMFLOAT4 rayDirection{ 0.0f, -1.0f,0.0f, 1.0f }; // �������̃��C
		//DirectX::XMFLOAT4 start = { position.x, position.y , position.z, 1.0f };
		DirectX::XMFLOAT4 end = { position.x + mx, position.y + stepOffset, position.z + mz,1.0f };
		// ���C�L���X�g�̌��ʂ��i�[����ϐ�
		DirectX::XMFLOAT4 hitPoint;
		DirectX::XMFLOAT3 hitNormal;
		DirectX::XMFLOAT3 Normal = { 0,1,0 };
		std::string hitMesh;
		std::string hitMaterial;
		// ���C�L���X�g�ɂ��ǔ���
		HitResult hit;
		ModelType modeltype1 = StageMain::Instance().GetModel()->GetModelType();
		
		if (StageManager::Instance().Raycast(rayStart, end, hitPoint, hitNormal, hitMesh, hitMaterial, ModelType::StageMain, e1))
		{
			
				// �ǂ܂ł̃x�N�g��
				DirectX::XMVECTOR Start = DirectX::XMLoadFloat4(&rayStart);
				DirectX::XMFLOAT4 resultStart;
				DirectX::XMStoreFloat4(&resultStart, Start);
				DirectX::XMVECTOR End = DirectX::XMLoadFloat4(&end);
				DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

				// �ǂ̖@��
				DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hitNormal);
				// ���˃x�N�g����@���Ɏˉe
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);
				// �␳�ʒu�̌v�Z
				DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
				DirectX::XMFLOAT4 collectPosition;
				DirectX::XMStoreFloat4(&collectPosition, CollectPosition);
				// �ǂ�������փ��C�L���X�g
				//HitResult hit2;
				DirectX::XMFLOAT4 hitPoint1;
				if (!DirectX::XMVector4Equal(CollectPosition, Start) &&
					!StageManager::Instance().Raycast(resultStart, collectPosition, hitPoint1, hitNormal, hitMesh, hitMaterial, ModelType::StageMain, e1))
				{

					// �ǂ�������ŕǂɓ�����Ȃ�������␳�ʒu�Ɉړ�
					position.x = collectPosition.x;
					position.z = collectPosition.z;

				}
				else
				{

					position.x = hitPoint.x;
					position.z = hitPoint.z;

				}
			
		}

		else
		{
			// �ړ�
			position.x += mx;
			position.z += mz;
		}

	}

}

