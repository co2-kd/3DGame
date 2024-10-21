#include "GameScene.h"
#include"../SceneManager.h"

#include"../../GameObject/Chara/Player/Player.h"
#include"../../GameObject/Chara/Player/Player_Battery.h"
#include"../../GameObject/Chara/Player/Player_Cannon.h"
#include"../../GameObject/Chara/Player/Player_Minigun.h"
#include"../../GameObject/Chara/Player/Player_Nuke-tube.h"
#include"../../GameObject/Chara/Enemy/Enemy.h"
#include"../../GameObject/Chara/Enemy/Drone/Drone.h"
#include"../../GameObject/Ground/Ground.h"
#include"../../GameObject/Back/Back.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"


void GameScene::Init()
{

	//地面（仮）
	std::shared_ptr<Ground> _ground;
	_ground = std::make_shared<Ground>();
	_ground->Init();
	AddObject(_ground);

	//背景（仮）
	std::shared_ptr<Back> _back;
	_back = std::make_shared<Back>();
	_back->Init();
	AddObject(_back);

	//プレイヤー
	std::shared_ptr<Player> _player = std::make_shared<Player>();
	_player->Init();
	AddObject(_player);

	//プレイヤー砲台
	std::shared_ptr<Player_Battery> _player_battery = std::make_shared<Player_Battery>();
	_player_battery->Init();
	AddObject(_player_battery);

	std::shared_ptr<Player_Cannon> _player_cannon = std::make_shared<Player_Cannon>();
	_player_cannon->Init();
	AddObject(_player_cannon);

	std::shared_ptr<Player_Minigun> _player_minigun = std::make_shared<Player_Minigun>();
	_player_minigun->Init();
	AddObject(_player_minigun);

	std::shared_ptr<Player_Nuketube> _player_nuketube = std::make_shared<Player_Nuketube>();
	_player_nuketube->Init();
	AddObject(_player_nuketube);

	//敵
	//std::shared_ptr<Enemy> _enemy = std::make_shared<Enemy>();
	//_enemy->Init();
	//AddObject(_enemy);
	
	std::shared_ptr<Drone> _drone = std::make_shared<Drone>();
	_drone->Init();
	AddObject(_drone);


	//カメラの追加
	std::shared_ptr<TPSCamera> _camera = std::make_shared<TPSCamera>();
	_camera->Init();
	m_objList.push_back(_camera);

	//セッター関連
	_camera->SetTarget(_player);
	_camera->RegistHitObject(_ground);
	_player->RegistHitObject(_ground);
	_player->SetCamera(_camera);
	_player->SetBattery(_player_battery);
	_player_battery->SetCamera(_camera);
	_player_battery->SetParent(_player);
	_player_battery->SetCannon(_player_cannon);
	_player_battery->SetMinigun(_player_minigun);
	_player_battery->SetNuketube(_player_nuketube);
	_player_cannon->SetParent(_player_battery);
	_player_minigun->SetParent(_player_battery);
	_player_minigun->SetCamera(_camera);
	_player_nuketube->SetParent(_player_battery);

	//BGM再生
	KdAudioManager::Instance().Play("Asset/Sounds/Game.wav", true);
}

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
			KdAudioManager::Instance().SoundReset();
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
