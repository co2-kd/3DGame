#pragma once

#include"../../CharaBase.h"
class ExpDrone : public CharaBase
{
public:
	ExpDrone() {}
	~ExpDrone()override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	void OnHit(const int _dmg)override;

private:
	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();

	//爆発時の判定
	void ExpCollision();

	//索敵フラグ
	bool m_searchFlg = false;
	const float m_searchArea = 100;

	//爆発フラグ
	bool m_expFlg = false;

	//速度
	float m_speed = 0.9f;

	const int m_ascountMax = 180;
	int m_ascount = m_ascountMax;

	int m_expcount = 300;

	//HP
	const int m_hpMax = 20;
	int m_hp = m_hpMax;

	//ダメージ
	const int m_dmg = 1;
};