﻿#pragma once

class CameraBase;
class Player : public KdGameObject
{
public:

	Player()						{}
	~Player()			override	{}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	void DrawLit()			override;
	void GenerateDepthMapFromLight() override;
	void DrawSprite()       override;

	void ImguiUpdate()override;

	void SetCamera(const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

	void RegistHitObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_wpHitObjectList.push_back(object);
	}

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();

	void UpdateJump();

	KdTexture m_boostTex;
	KdTexture m_boostbackTex;
	KdTexture m_boostflontTex;

	Math::Rectangle m_boostRc;
	Math::Rectangle m_boostbackRc;

	Math::Matrix m_boostMat;

	std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;
	std::weak_ptr<CameraBase>			m_wpCamera;
	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};
	Math::Vector3						m_worldRot;

	//キャラが向いている方向
	Math::Vector3 m_dir = Math::Vector3::Zero;

	float m_speed = 0;
	Math::Vector3 m_moveVec = Math::Vector3::Zero;
	Math::Vector3 m_pos = Math::Vector3::Zero;

	float m_gravity = 0;
	const float m_gravityPow = 0.15f;
	const float m_gravityMax = 5.0f;

	//ホバー
	const float m_hoverspeed = 0.6f;
	
	//ジャンプ
	bool m_jumpFlg = false;
	float m_jumpspeed = 0;
	const float m_jumpspeedPow = 0.3f;
	const float m_jumpspeedMax = 3.0f;

	//ステップ・ブースト
	bool m_stepFlg = false;

	float m_stepspeed = 0;
	const float m_stepspeedPow = 15.0f;

	float m_boostspeed = 0;
	const float m_boostspeedPow = 3.0f;

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