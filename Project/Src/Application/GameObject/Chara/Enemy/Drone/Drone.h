#pragma once

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

};