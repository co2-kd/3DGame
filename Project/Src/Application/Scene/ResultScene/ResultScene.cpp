#include "ResultScene.h"
#include "../SceneManager.h"

#include"../../GameObject/Scene/Title/Title.h"

void ResultScene::Event()
{
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
	{
		if (!m_keyFlg)
		{
			m_keyFlg = true;
			KdAudioManager::Instance().StopAllSound();
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Title
			);
		}
	}
	else
	{
		m_keyFlg = false;
	}
}

void ResultScene::Init()
{
	ShowCursor(true);

	std::shared_ptr<Title> _title;
	_title = std::make_shared<Title>();
	_title->Init();
	AddObject(_title);

	//BGM再生
	KdAudioManager::Instance().Play("Asset/Sounds/Title.wav", true);
}
