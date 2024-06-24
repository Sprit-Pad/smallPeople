#pragma once

#include "Scene.h"
#include <thread>
#include "framework.h"
//ローディングシーン
class SceneLoading : public Scene {


public:
	SceneLoading(std::unique_ptr<Scene>nextScene):nextScene(std::move(nextScene)){}

	~SceneLoading()override{}

	//初期化
	void Initialize()override;

	//終了化
	void Finalize()override;

	//更新処理
	void Update(float elapsedTime)override;

	//描画処理
	void Render()override;

private:

	//ローディングスレッド
	static void LoadingThread(SceneLoading* scene);

private:

	float angle = 0.0f;
	std::unique_ptr<Scene> nextScene = nullptr;
	std::unique_ptr<std::thread> thread = nullptr;
	



};