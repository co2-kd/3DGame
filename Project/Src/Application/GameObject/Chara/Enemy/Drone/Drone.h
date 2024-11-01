﻿#pragma once

#include"../../CharaBase.h"
class Drone : public CharaBase
{
public:
	Drone() {}
	~Drone()override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	void DrawLit()override;

	void OnHit()			override;

private:
	void UpdateRotate(const Math::Vector3& srcMoveVec);

	//本体から銃口へのローカル行列
	Math::Matrix m_localmuzzleMat;

	Math::Matrix m_APMuzzleMat;

	int m_hp = 10;
};