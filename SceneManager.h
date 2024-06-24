#pragma once
#include "Scene.h"

//�V�[���}�l�[�W���[
class SceneManager
{

private:
	 SceneManager(){}
	~SceneManager(){}

public:
	//�B��̃C���X�^���X�擾
	static SceneManager& Instance() 
	{
		static SceneManager instance;
		return instance;
	}

	void Update(float elapsedTime);

	void Render();

	void Clear();

	void ChangeScene(std::unique_ptr<Scene> scene);

private:
	//���݂̃V�[��
	std::unique_ptr<Scene> currentScene ;
	//���̃V�[��
	std::unique_ptr<Scene> nextScene ;








};
