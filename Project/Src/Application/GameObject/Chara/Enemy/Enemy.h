#pragma once
#include"../CharaBase.h"
class Enemy : public CharaBase
{
public:
	Enemy() {}
	~Enemy()override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

private:

};