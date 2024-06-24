#pragma once
#include <DirectXMath.h>
#include "high_resolution_timer.h"
 
class Character {

public:
	Character() {};
	virtual ~Character() {}

	//行列更新処理
	void UpdateTransform();

	//位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	//位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

	//回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }

	//回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

	//スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	//半径取得
	float GetRadius() const { return radius; }

	//地面に接触しているか
	bool IsGround() const { return isGround; }


	float GetHeight()const { return height; }
	//ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime);
	//衝撃を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

	//健康状態を取得
	int GetHealth()const { return health; }

	//最大健康状態を取得
	int GetMaxHealth()const { return maxHealth; }

	
	void DebugImgui();


protected:


	void Move(float vx, float vz, float speed);

	void jump(float speed);

	void UpdateVelocity(float elapsedTime);

	void Turn(float elaspsedTime, float vx, float vz, float speed);

	void UpdateInvincibleTimer(float elapsedTime);
private:
	//水平速力更新処理
	void UpdateVerticalVelocity(float elapsedFrame);
	//水平移動更新処理
	void UpdateVerticalMove(float elapsedTime);
	//水平速力更新処理
	void UpdateHorizontalVelocity(float elapsedFrame);
	//水平移動更新処理
	void UpdateHorizontalMove(float elapsedTime);

	virtual void OnLanding() {}
	virtual void OnDamaged() {}
	virtual void OnDead() {}

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,
									 0,1,0,0,
									 0,0,1,0,
									 0,0,0,1 };
	high_resolution_timer			timer;
	float radius = 0.5f;
	float height = 2.0f;
	DirectX::XMFLOAT4 material_color = {0,0,0,1};
	float gravity = -1.0f;
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	bool isGround = false;
	bool isClimbingSlope = true;
	int health = 5;
	int maxHealth = 5;

	float friction = 0.5f;

	float invincibleTimer = 1.0f;

	float acceleration = 1.0f;
	float maxMoveSpeed = 5.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;
	float airControl = 0.3f;
	//
	float stepOffset = 2.0f;
	float slopeRate = 1.0f;
	int animationclip = 0;
	float timerInter=0;
	int		frames = 0;
	float	averageTime = 0;
	bool e =false;
	bool e1 = true;
};