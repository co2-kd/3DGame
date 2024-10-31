#pragma once
#include"../../TurretBase.h"

class TurretBase;
class T_Laser : public TurretBase
{
public:

	T_Laser() {}
	~T_Laser()			override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	//ゲッター
	

private:

	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;

	//本体から銃口へのローカル行列
	Math::Matrix m_localmuzzleMat;


	//ステートパターン関係
private:
	class ActionStateBase
	{
	public:
		virtual ~ActionStateBase() {}

		virtual void Enter(T_Laser& owner) {}
		virtual void Update(T_Laser& owner) {}
		virtual void Exit(T_Laser& owner) {}
	};


	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;

};