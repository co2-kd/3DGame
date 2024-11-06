#pragma once
#include"../../../Chara/CharaBase.h"

class TurretBase : public CharaBase
{
public:
	TurretBase() {}
	virtual ~TurretBase()override {}

	virtual void Update() override;

	void SetAPMatrix(const Math::Matrix _localMat)override
	{
		m_localMat = _localMat;
	}

protected:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	Math::Matrix m_TransAPMat;

	//本体から銃口へのローカル行列
	Math::Matrix m_localmuzzleMat;

	bool m_ShotFlg = false;
	const int m_Shotcool = 0;
	int m_Shotcount = 0;
};