#include "ResultScene.h"
#include "../SceneManager.h"

#include"../../GameObject/Scene/Result/Result.h"

void ResultScene::Event()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
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


	std::shared_ptr<Result> _result;
	_result = std::make_shared<Result>();
	_result->Init();
	AddObject(_result);

	m_keyFlg = true;
}
