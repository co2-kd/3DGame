﻿#pragma once
#include"../CharaBase.h"

class CameraBase;
class Player_Battery;
class Player : public CharaBase
{
public:

	Player()						{}
	~Player()			override	{}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	//void DrawLit()			override;
	//void GenerateDepthMapFromLight() override;
	void DrawSprite()       override;

	void ImguiUpdate()override;

	//セッター
	void SetCamera(const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

	void SetBattery(const std::shared_ptr<Player_Battery>& battery)
	{
		m_wpBattery = battery;
	}

	void OnHit(const int _dmg)override;

	//ゲッター
	const Math::Matrix GetAPBatteryMatrix()const { return m_APBatteryMat; }//アタッチポイント

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();

	void UpdateJump();

	KdTexture m_boostTex;
	KdTexture m_boostbackTex;
	KdTexture m_boostflontTex;
	KdTexture m_tex;

	Math::Rectangle m_boostRc;
	Math::Rectangle m_boostbackRc;

	Math::Matrix m_boostMat;

	//std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;
	std::weak_ptr<CameraBase>			m_wpCamera;

	Math::Vector3 m_velocity;//慣性
	const float m_inertiaFactor = 0.965f;  // 慣性の強さ、0.9 などで減衰させる
	const float m_skyinertiaFactor =0.99f;  // 慣性の強さ、0.9 などで減衰させる

	//プレイヤー座標補正
	const float m_adjustHeight = 1.0f;
	//重力
	const float m_gravity = 1.5f;
	const float m_floatingWeak = 0.03f;
	const float m_floatingMin = 0.0f;
	float m_floating = 0;

	//ホバー
	const float m_hoverspeed = 0.3f;
	
	//ジャンプ
	bool m_jumpFlg = false;
	float m_jumpspeed = 0;
	const float m_jumpspeedPow = 0.1f;
	const float m_jumpspeedMax = 1.8f;
	//デバッグ
	//const float m_jumpspeedMax = 2.8f;
	const float m_jumpspeedStart = 0.5f;

	//ステップ・ブースト
	bool m_stepFlg = false;
	bool m_stepbeginFlg = false;
	const int m_stepFlamefixed = 15;
	int m_stepFlame = m_stepFlamefixed;


	const float m_stepspeed = 0.8f;
	//デバッグ
	//const float m_stepspeed = 2.8f;

	const float m_boostspeed = 0.5f;
	//デバッグ
	//const float m_boostspeed = 2.5f;

	//ドライブブースト
	bool m_driveFlg = false;

	int waitdrivetime = 0;

	
	float m_drivespeed = 0;
	const float m_drivespeedPow = 30.0f;

	float m_driveboostspeed = 0;
	const float m_driveboostspeedPow = 10.0f;

	//ブーストゲージ
	int m_boostgauge = 800;
	const int m_boostgaugeMax = 800;

	bool m_overheatFlg = false;

	bool m_boostFlg = false;

	bool m_groundFlg = false;

	//HP
	const int m_hpMax = 100;
	int m_hp = m_hpMax;

	//アタッチポイント関係
	std::weak_ptr<Player_Battery> m_wpBattery;
	Math::Matrix m_APBatteryMat = {};


//ステートパターン関係
private:
	class ActionStateBase
	{
	public:
		virtual ~ActionStateBase() {}

		virtual void Enter(Player& owner) {}
		virtual void Update(Player& owner) {}
		virtual void Exit(Player& owner) {}
	};
	
	//待機状態
	class ActionIdle : public ActionStateBase
	{
	public:
		~ActionIdle() {}

		void Enter(Player& owner) override;
		void Update(Player& owner) override;
		void Exit(Player& owner) override;
	};

	//ジャンプ状態
	class ActionJump : public ActionStateBase
	{
	public:
		~ActionJump() {}

		void Enter(Player& owner) override;
		void Update(Player& owner) override;
		void Exit(Player& owner) override;
	};

	//前進状態
	class ActionMove : public ActionStateBase
	{
	public:
		~ActionMove() {}

		void Enter(Player& owner) override;
		void Update(Player& owner) override;
		void Exit(Player& owner) override;
	};

	//ステップ状態
	class ActionStep : public ActionStateBase
	{
	public:
		~ActionStep() {}

		void Enter(Player& owner) override;
		void Update(Player& owner) override;
		void Exit(Player& owner) override;
	};

	//ドライブブースト状態
	class ActionDrive : public ActionStateBase
	{
	public:
		~ActionDrive() {}

		void Enter(Player& owner) override;
		void Update(Player& owner) override;
		void Exit(Player& owner) override;
	};


	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;

};