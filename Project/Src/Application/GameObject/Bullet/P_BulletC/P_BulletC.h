#pragma once
#include"../BulletBase.h"

class P_BulletC :public BulletBase
{
public:
	P_BulletC(){}
	~P_BulletC()override {}

	void Init() override;

	void Update()override;

	void DrawBright() override;


private:
	// 当たり判定
	void UpdateCollision();

	//着弾時の爆発
	void ExpCollision();

	bool m_expFlg = false;

	//速度
	const float m_speed = 2.0f;

	//生存時間
	int m_timelimit;

	//ダメージ
	const int m_dmg = 10;
};