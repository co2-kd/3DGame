#pragma once

#include"../BaseScene/BaseScene.h"

class GameScene : public BaseScene
{
public :

	GameScene() { Init(); }
	~GameScene() {}

private:

	void Event() override;
	void Init() override;

	std::shared_ptr<KdGameObject> m_player;
	std::shared_ptr<KdGameObject> m_drone2;
	std::shared_ptr<KdGameObject> m_drone3;
	std::shared_ptr<KdGameObject> m_turret;
};
