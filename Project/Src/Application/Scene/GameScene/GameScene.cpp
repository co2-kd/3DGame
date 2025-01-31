﻿#include "GameScene.h"
#include"../SceneManager.h"

#include"../../GameObject/Chara/Player/Player.h"
#include"../../GameObject/Chara/Player/Player_Battery.h"
#include"../../GameObject/Chara/Player/Player_Cannon.h"
#include"../../GameObject/Chara/Player/Player_Minigun.h"
#include"../../GameObject/Chara/Player/Player_Nuke-tube.h"

#include"../../GameObject/Chara/Enemy/Enemy.h"
#include"../../GameObject/Chara/Enemy/Wall/Wall.h"
#include"../../GameObject/Chara/Enemy/Drone/Drone.h"
#include"../../GameObject/Chara/Enemy/ExpDrone/ExpDrone.h"
#include"../../GameObject/Chara/Enemy/Turret/T_Pedestal.h"
#include"../../GameObject/Chara/Enemy/Turret/T_Leg.h"
#include"../../GameObject/Chara/Enemy/Turret/T_Type/T_Laser/T_Laser.h"

#include"../../GameObject/Ground/Stage1/Stage1.h"
#include"../../GameObject/Ground/Stage1/Stage1Col.h"
#include"../../GameObject/Back/Back.h"

#include"../../GameObject/Camera/TPSCamera/TPSCamera.h"


void GameScene::Init()
{
	ShowCursor(false);

	//画面全体が暗いときに触る
	//環境光(アンビエントライト)※デフォルト値は0.3					  	 ↓↓↓↓
	KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 0.4,0.4,0.4,1 });


	//地面（仮）
	std::shared_ptr<Stage1> _stage1;
	_stage1 = std::make_shared<Stage1>();
	_stage1->Init();
	AddObject(_stage1);
	//天井
	std::shared_ptr<Stage1Col> _stage1col;
	_stage1col = std::make_shared<Stage1Col>();
	_stage1col->Init();
	AddObject(_stage1col);
	//壁
	std::shared_ptr<Wall> _wall = std::make_shared<Wall>();
	_wall->SetPos({ 0,-60,490 });
	_wall->Init();
	AddObject(_wall);

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
	//ドローン
	std::shared_ptr<Drone> _drone = std::make_shared<Drone>();
	_drone->SetPos({ 0,-80,650 });
	_drone->Init();
	AddObject(_drone);

	//自爆ドローン
	std::shared_ptr<ExpDrone> _expdrone = std::make_shared<ExpDrone>();
	_expdrone->Init();
	_expdrone->SetPos({ 50,-20,360 });
	AddObject(_expdrone);


	//タレット
	std::shared_ptr<T_Pedestal> _t_pedestal = std::make_shared<T_Pedestal>();
	_t_pedestal->Init();
	_t_pedestal->SetPos({-150,-25.0f,150});
	AddObject(_t_pedestal);
	std::shared_ptr<T_Leg> _t_leg = std::make_shared<T_Leg>();
	_t_leg->Init();
	AddObject(_t_leg);

	std::shared_ptr<T_Laser> _t_laser = std::make_shared<T_Laser>();
	_t_laser->Init();
	AddObject(_t_laser);


	//カメラの追加
	std::shared_ptr<TPSCamera> _camera = std::make_shared<TPSCamera>();
	_camera->Init();
	m_objList.push_back(_camera);

	// カメラ情報をセット
	KdEffekseerManager::GetInstance().SetCamera(_camera->GetCamera());

	//セッター関連
	_camera->SetTarget(_player);
	_camera->RegistHitObject(_stage1);
	_camera->RegistHitObject(_stage1col);
	_camera->RegistHitObject(_wall);
	_camera->RegistHitObject(_t_pedestal);

	_player->RegistHitObject(_t_pedestal);
	_player->RegistHitObject(_stage1);
	_player->RegistHitObject(_stage1col);
	_player->RegistHitObject(_wall);
	_player->SetCamera(_camera);
	_player->SetBattery(_player_battery);

	_player_battery->SetCamera(_camera);
	_player_battery->SetParent(_player);
	_player_battery->SetCannon(_player_cannon);
	_player_battery->SetMinigun(_player_minigun);
	_player_battery->SetNuketube(_player_nuketube);

	_player_cannon->SetParent(_player_battery);
	_player_cannon->SetCamera(_camera);

	_player_minigun->SetParent(_player_battery);
	_player_minigun->SetCamera(_camera);

	_player_nuketube->SetParent(_player_battery);

	_drone->SetTarget(_player);
	_drone->RegistHitObject(_stage1);

	_expdrone->SetTarget(_player);

	_t_pedestal->SetT_Leg(_t_leg);

	_t_leg->SetParent(_t_pedestal);
	_t_leg->SetTarget(_player);
	_t_leg->SetTurretBase(_t_laser);

	_t_laser->SetParent(_t_leg);
	_t_laser->SetTarget(_player);


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
			KdAudioManager::Instance().StopAllSound();
			SceneManager::Instance().SetNextScene
			(
				SceneManager::SceneType::Result
			);
		}
	}
	else
	{
		m_keyFlg = false;
	}
}
