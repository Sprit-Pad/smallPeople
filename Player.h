#include "Character.h"

#include "Collision.h"

enum Animation
{

	Anim_Attack,
	Anim_Death,
	Anim_Falling,
	Anim_GetHit1,
	Anim_GetHit2,
	Anim_Idle,
	Anim_Jump,
	Anim_Jump_Flip,
	Anim_Landing,
	Anim_Revive,
	Anim_Running,
	Anim_Walking
};
enum AnimationState {
	Blend,
	Normal
};

//プレイヤー
class Player :public Character
{

public:
	Player();
	~Player() override;

	//デバッグ用GUI描画
	//void DrawDebugGUI();

	//更新処理
	void Update(float elapsedTime);

	void DebugIMGUI();

	//描画処理
	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>immediate_context,
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[],
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[]
		, Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[],DirectX::XMFLOAT4 material_color);

	void UpdateAnimation(float elapsedTime,  int clip_index = 0, float animationBlendTime=0, float animationBlendSeconds=0);

	static Player& Instance();
	bool update = true;

protected:

	void OnLanding() override;
	void OnDamaged()override;
	void OnDead()override;

	bool InputMove(float elapsedTime);

private:
	void UpdateLandState(float elapsedTime);
	void UpdateIdleState(float elapsedTime);
	void TransitionJumpState();
	void UpdateMoveState(float elapsedTime);
	bool InputAttackState();
	void TransitionLandState();
	void UpdateJumpState(float elapsedTime);
	void TransitionMoveState();
	bool InputJump();
	void TransitionAttackState();
	void UpdateAttackState(float elapsedTime);
	void SetanimationEnd(const bool& animationEnd) { this->animationEnd = animationEnd; }
    bool GetanimationEnd()const { return animationEnd; }
	DirectX::XMFLOAT3 GetMoveVec()const;
	bool IsPlayAnimation();
	void TransitionIdleState();
	void SetAnimationBlendTime(const float& animationBlendTime) { this->AnimationBlendTime = animationBlendTime; }
	void SetanimationBlendSeconds(const float& animationBlendSeconds) { this->AnimationBlendSeconds = animationBlendSeconds; }
	int GetAnimationBlendTime()const { return AnimationBlendTime; }
	int GetanimationBlendSeconds()const { return AnimationBlendSeconds; }

	//ステート
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Damage,
		Death,
		Revive,
		Attack,
	};


public:

	int animationclip = 0;
	State state = State::Idle;
	
private:

	float  timeData = 0;

	float moveSpeed = 5.0f;

	float turnSpeed = DirectX::XMConvertToRadians(720);

	float jumpSpeed = 10.0f;

	int jumpCount = 0;
	int jumpLimit = 2;
	
	int timer = 0;	
	//FBXSDK
	std::unique_ptr<skinned_mesh> skinned_meshes[8];
	int y = 0;
	float leftRadius = 0.2f;
	float enemyleftRadius1 = 0.1f;
	bool attackCollisionFlag = false;
	animation::keyframe keyframe1 ;
	animation::keyframe keyframe2;
	float animationTime = 0;
	float AnimationBlendTime=0;
	float AnimationBlendSeconds=0.2f;
	bool blend=false;
	int animation_index=0;
	bool animationEnd=false;
	float animationcurrentTimer = 0;
};

