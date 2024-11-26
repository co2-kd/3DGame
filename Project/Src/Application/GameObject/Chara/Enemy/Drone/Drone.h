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

	//モデル情報
	std::shared_ptr<KdModelWork>m_spLightModel = nullptr;
	
	//本体から銃口へのローカル行列
	Math::Matrix m_localmuzzleMat;

	Math::Matrix m_APMuzzleMat;

	//索敵フラグ
	bool m_searchFlg = false;
	const float m_searchArea = 100;

	//HP
	const int m_hpMax = 20;
	int m_hp = m_hpMax;


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


	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;

};