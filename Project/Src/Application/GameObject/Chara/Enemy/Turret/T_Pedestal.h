#pragma once

#include"../../CharaBase.h"
class T_Leg;
class T_Pedestal : public CharaBase
{
public:
	T_Pedestal() {}
	~T_Pedestal()override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	void DrawLit()			override;



	void SetT_Leg(const std::shared_ptr<T_Leg>& t_leg)
	{
		m_wpLeg = t_leg;
	}

private:

	

	//アタッチポイント関係
	std::weak_ptr<T_Leg> m_wpLeg;
	Math::Matrix m_APLegMat = {};

	
};