//#include "Graphics/Graphics.h"
#include "SceneLoading.h"
//#include "Input/Input.h"
#include "SceneManager.h"


//初期化
void SceneLoading::Initialize()
{
  //スプライト初期化
	//sprite = new Sprite("Data/Sprite/LoadingIcon.png");

  //スレッド開始
	
	thread = std::make_unique<std::thread>(LoadingThread, this);
	//thread->join();
}

//終了化
void SceneLoading::Finalize()
{

 //スレッド終了化
	if (thread != nullptr) 
	{
		thread->join();
	}
	
}

//更新処理
void SceneLoading::Update(float elapsedTime)
{


	constexpr float speed = 180;
	//回転
	angle += speed * elapsedTime;

	//次のシーンの準備が完了したらシーンを切り替える
	if (nextScene->IsReady()) 
	{
		SceneManager::Instance().ChangeScene(std::move(nextScene));
		nextScene = nullptr;
	}



}

//描画処理
void SceneLoading::Render()
{

	



}

void SceneLoading::LoadingThread(SceneLoading* scene)
{

	//COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);
	

	//次のシーンの初期化を行う
	scene->nextScene->Initialize();

	//スレッドが終わる前にCOM関連の終了化
	CoUninitialize();
	
	//次のシーンの準備完了設定
	scene->nextScene->SetReady();


}
