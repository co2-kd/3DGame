#pragma once
#include"../BulletBase.h"

class P_BulletM :public BulletBase
{
public:
	P_BulletM(){}
	~P_BulletM()override {}

	void Init() override;

	void Update()override;

	void DrawLit()override {}
	void DrawUnLit()override;
	void DrawBright() override;

	void OnHit()override;

private:
	// 当たり判定
	void UpdateCollision();

	const float m_speed = 2.0f;
	int m_timelimit;
};