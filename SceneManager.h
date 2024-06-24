#pragma once
#include "Scene.h"

//シーンマネージャー
class SceneManager
{

private:
	 SceneManager(){}
	~SceneManager(){}

public:
	//唯一のインスタンス取得
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
	//現在のシーン
	std::unique_ptr<Scene> currentScene ;
	//次のシーン
	std::unique_ptr<Scene> nextScene ;








};
