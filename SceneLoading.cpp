//#include "Graphics/Graphics.h"
#include "SceneLoading.h"
//#include "Input/Input.h"
#include "SceneManager.h"


//������
void SceneLoading::Initialize()
{
  //�X�v���C�g������
	//sprite = new Sprite("Data/Sprite/LoadingIcon.png");

  //�X���b�h�J�n
	
	thread = std::make_unique<std::thread>(LoadingThread, this);
	//thread->join();
}

//�I����
void SceneLoading::Finalize()
{

 //�X���b�h�I����
	if (thread != nullptr) 
	{
		thread->join();
	}
	
}

//�X�V����
void SceneLoading::Update(float elapsedTime)
{


	constexpr float speed = 180;
	//��]
	angle += speed * elapsedTime;

	//���̃V�[���̏���������������V�[����؂�ւ���
	if (nextScene->IsReady()) 
	{
		SceneManager::Instance().ChangeScene(std::move(nextScene));
		nextScene = nullptr;
	}



}

//�`�揈��
void SceneLoading::Render()
{

	



}

void SceneLoading::LoadingThread(SceneLoading* scene)
{

	//COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);
	

	//���̃V�[���̏��������s��
	scene->nextScene->Initialize();

	//�X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();
	
	//���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();


}
