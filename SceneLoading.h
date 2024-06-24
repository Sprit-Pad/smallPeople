#pragma once

#include "Scene.h"
#include <thread>
#include "framework.h"
//���[�f�B���O�V�[��
class SceneLoading : public Scene {


public:
	SceneLoading(std::unique_ptr<Scene>nextScene):nextScene(std::move(nextScene)){}

	~SceneLoading()override{}

	//������
	void Initialize()override;

	//�I����
	void Finalize()override;

	//�X�V����
	void Update(float elapsedTime)override;

	//�`�揈��
	void Render()override;

private:

	//���[�f�B���O�X���b�h
	static void LoadingThread(SceneLoading* scene);

private:

	float angle = 0.0f;
	std::unique_ptr<Scene> nextScene = nullptr;
	std::unique_ptr<std::thread> thread = nullptr;
	



};