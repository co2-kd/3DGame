#pragma once
#include"../BulletBase.h"

class E_D_Bullet :public BulletBase
{
public:
	E_D_Bullet(){}
	~E_D_Bullet()override {}

	void Init() override;

	void Update()override;

	void DrawBright() override;

private:
	// 当たり判定
	void UpdateCollision();

	const float m_speed = 1.0f;
	int m_timelimit;

	const int m_dmg = 1;
};