#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <filesystem>
#include "sprite.h"
#include "sprite_batch.h"
#include <wrl.h>
#include <memory>
#include "shader.h"
#include "texture.h"
#include "geometric_primitive.h"
#include "static_mesh.h"
#include "skinned_mesh.h"
#include "framebuffer.h"
#include "fullscreen_quad.h"
// TODO HUSK.00
#include "husk_particles.h"

// PARTICLE
#include "particles.h"
#include "Camera/CameraController.h"

//シーン
class Scene
{
public:
	Scene(){}
	virtual ~Scene(){}

	//初期化
	virtual void Initialize() = 0;

	//終了化
	virtual void Finalize() = 0;

	//更新処理
	virtual void  Update(float elapsedTime) = 0;

	//描画処理
	virtual void Render() = 0;

	//準備完了しているか
	bool IsReady()const { return ready; }

	//準備完了設定
	void SetReady() { ready = true; }






private:
	bool ready = false;








};