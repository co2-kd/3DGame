#include "GameScene.h"
#include"../SceneManager.h"

#include"../../GameObject/Chara/Player/Player.h"
#include"../../GameObject/Chara/Player/Player_Battery.h"
#include"../../GameObject/Chara/Enemy/Enemy.h"
#include"../../GameObject/Ground/Ground.h"
#include"../../GameObject/Back/Back.h"
#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"

void GameScene::Event()
{
	//{
	//	//円形描画テスト用（消す）
	//	KdTexture Tex;
	//	Math::Rectangle Rect;
	//	Math::Color Color;
	//	float angle = 0.0f;
	//	Tex.Load("Asset/Textures/test.png");
	//	Color = { 1,1,1,1 };
	//	Rect = { 0,0,128,128 };
	//	KdShaderManager::Instance().m_spriteShader.DrawTexAnime(&Tex, 0, 0, 128, 128, angle, &Rect, &Color);
	//}
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		if (!m_keyFlg)
		{
			m_keyFlg = true;
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

void GameScene::Init()
{

	//地面（仮）
	std::shared_ptr<Ground> _ground;
	_ground = std::make_shared<Ground>();
	_ground->Init();
	AddObject(_ground);

	////背景（仮）
	//std::shared_ptr<Back> _back;
	//_back = std::make_shared<Back>();
	//_back->Init();
	//AddObject(_back);

	//プレイヤー
	std::shared_ptr<Player> _player;
	_player = std::make_shared<Player>();
	_player->Init();
	AddObject(_player);

	//プレイヤー砲台
	std::shared_ptr<Player_Battery> _player_battery;
	_player_battery = std::make_shared<Player_Battery>();
	_player_battery->Init();
	AddObject(_player_battery);

	//エネミー
	std::shared_ptr<Enemy> _enemy;
	_enemy = std::make_shared<Enemy>();
	_enemy->Init();
	AddObject(_enemy);

	//カメラの追加
	std::shared_ptr<TPSCamera> _camera;
	_camera = std::make_shared<TPSCamera>();
	_camera->Init();
	m_objList.push_back(_camera);


	_camera->SetTarget(_player);
	_camera->RegistHitObject(_ground);
	_player->RegistHitObject(_ground);
	_player->SetCamera(_camera);
	_player->SetBattery(_player_battery);
	_player_battery->SetParent(_player);
}
