#pragma once

#include"../../CharaBase.h"
class Drone : public CharaBase
{
public:
	Drone() {}
	~Drone()override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	void DrawBright()override;

	void OnHit(const int _dmg)override;

private:
	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();

	//弾発射
	void ShotBullet();

	//モデル情報
	std::shared_ptr<KdModelWork>m_spLightModel = nullptr;
	
	//本体から銃口へのローカル行列
	Math::Matrix m_localmuzzleMat;

	Math::Matrix m_APMuzzleMat;

	//初期座標
	Math::Vector3 m_startPos;

	//索敵フラグ
	bool m_searchFlg = false;
	const float m_searchAreaSearch = 100;//索敵状態の範囲
	const float m_searchAreaBattle = 200;//交戦状態の範囲
	float m_searchArea = m_searchAreaSearch;

	//距離を入れる変数
	float m_length = 0;

	float m_speed = 0.05;

	const float m_keepAreaMax = 50; //最大目標距離
	const float m_keepAreaMin = 40; //最小目標距離
	const float m_keepcountMax = 120;//判定間隔カウント
	float m_keepcount = 0;

	//HP
	const int m_hpMax = 20;
	int m_hp = m_hpMax;

	//弾発射関係
	bool m_ShotFlg = false;
	int m_Shotcount = 0;
	const int m_Shotcool = 60;
	const int m_searchShotcoolMax = 20;
	int m_searchShotcool = m_searchShotcoolMax;


	//乱数生成器
	std::shared_ptr<KdRandomGenerator> m_RandomGen;

	//乱数用
	int m_bunbo = 1000;
	int m_rnd = 0;

	//重力
	const float m_adjustHeight = 5.0f;
	const float m_gravity = 0.3f;
	const float m_floatingWeak = 0.03f;
	const float m_floatingMin = 0.0f;
	float m_floating = 0;

	//ステートパターン関係
private:
	class ActionStateBase
	{
	public:
		virtual ~ActionStateBase() {}

		virtual void Enter(Drone& owner) {}
		virtual void Update(Drone& owner) {}
		virtual void Exit(Drone& owner) {}
	};

	//未発見状態
	class ActionSearch : public ActionStateBase
	{
	public:
		~ActionSearch() {}

		void Enter(Drone& owner) override;
		void Update(Drone& owner) override;
		void Exit(Drone& owner) override;
	};
	
	//交戦状態
	class ActionBattle : public ActionStateBase
	{
	public:
		~ActionBattle() {}

		void Enter(Drone& owner) override;
		void Update(Drone& owner) override;
		void Exit(Drone& owner) override;
	};
	
	//帰還状態
	class ActionReturn : public ActionStateBase
	{
	public:
		~ActionReturn() {}

		void Enter(Drone& owner) override;
		void Update(Drone& owner) override;
		void Exit(Drone& owner) override;
	};


	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;
	
//--------------------------------------------
//バトルステートパターン関係（2重ステート）
//--------------------------------------------
private:
	class BattleStateBase
	{
	public:
		virtual ~BattleStateBase() {}

		virtual void Enter(Drone& owner) {}
		virtual void Update(Drone& owner) {}
		virtual void Exit(Drone& owner) {}
	};

	//適正距離状態
	class RangeJust : public BattleStateBase
	{
	public:
		~RangeJust() {}

		void Enter(Drone& owner) override;
		void Update(Drone& owner) override;
		void Exit(Drone& owner) override;
	};
	
	//適性より近い状態
	class RangeFar : public BattleStateBase
	{
	public:
		~RangeFar() {}

		void Enter(Drone& owner) override;
		void Update(Drone& owner) override;
		void Exit(Drone& owner) override;
	};
	
	//適正より遠い状態
	class RangeNear : public BattleStateBase
	{
	public:
		~RangeNear() {}

		void Enter(Drone& owner) override;
		void Update(Drone& owner) override;
		void Exit(Drone& owner) override;
	};


	void ChangeBattleState(std::shared_ptr<BattleStateBase> nextState);
	std::shared_ptr<BattleStateBase> m_nowRange = nullptr;

};