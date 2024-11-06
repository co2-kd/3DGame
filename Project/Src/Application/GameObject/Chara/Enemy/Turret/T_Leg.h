#pragma once
#include"../../CharaBase.h"

class TurretBase;
class T_Leg : public CharaBase
{
public:

	T_Leg() {}
	~T_Leg()			override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	void SetAPMatrix(const Math::Matrix _localMat)override
	{
		m_localMat = _localMat;
	}

	void SetTurretBase(const std::shared_ptr<TurretBase>& turretbase)
	{
		m_wpTurretBase = turretbase;
	}

	//ゲッター
	const bool GetSearchFlg()const override { return m_searchFlg; }

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 当たり判定と
	//void UpdateCollision();

	//std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;

	bool m_searchFlg = false;
	const float m_searchArea = 50;

	//アタッチポイント関係
	std::weak_ptr<TurretBase> m_wpTurretBase;
	Math::Matrix m_APTurretMat = {};


	//ステートパターン関係
private:
	class ActionStateBase
	{
	public:
		virtual ~ActionStateBase() {}

		virtual void Enter(T_Leg& owner) {}
		virtual void Update(T_Leg& owner) {}
		virtual void Exit(T_Leg& owner) {}
	};


	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;

};