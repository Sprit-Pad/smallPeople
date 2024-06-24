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

//�V�[��
class Scene
{
public:
	Scene(){}
	virtual ~Scene(){}

	//������
	virtual void Initialize() = 0;

	//�I����
	virtual void Finalize() = 0;

	//�X�V����
	virtual void  Update(float elapsedTime) = 0;

	//�`�揈��
	virtual void Render() = 0;

	//�����������Ă��邩
	bool IsReady()const { return ready; }

	//���������ݒ�
	void SetReady() { ready = true; }






private:
	bool ready = false;








};