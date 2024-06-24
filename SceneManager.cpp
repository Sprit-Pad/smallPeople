#include "SceneManager.h"

void SceneManager::Update(float elapsedTime)
{

	if (nextScene != nullptr)
	{
		//古いシーン終了処理
		Clear();
		
		//新しいシーン設定
		currentScene = std::move(nextScene);
		nextScene = nullptr;
		//シーン初期化処理
		if (!currentScene->IsReady()) 
		{
			currentScene->Initialize();
		}
		
	}

	 if (currentScene != nullptr) 
	 {

		currentScene->Update(elapsedTime);
	 }
 
}

void SceneManager::Render()
{

	if (currentScene != nullptr)
	{
		currentScene->Render();

	}

}

void SceneManager::Clear()
{

	if (currentScene != nullptr)
	{

		currentScene->Finalize();
		currentScene = nullptr;
		
	}

}

void SceneManager::ChangeScene(std::unique_ptr<Scene> scene)
{
	nextScene = nullptr;
	nextScene = std::move(scene);
}
