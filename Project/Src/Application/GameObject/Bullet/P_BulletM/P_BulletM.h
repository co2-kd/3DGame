#pragma once
#include"../BulletBase.h"

class P_BulletM :public BulletBase
{
public:
	P_BulletM(){}
	~P_BulletM()override {}

	void Init() override;

	void Update()override;

	void DrawBright() override;



private:
	// 当たり判定
	void UpdateCollision();

	const float m_speed = 1.0f;
	int m_timelimit = 30;

	const int m_dmg = 1;
};