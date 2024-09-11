#pragma once

class Enemy : public KdGameObject
{
public:
	Enemy() {}
	~Enemy()override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;
	void DrawLit()			override;
	void GenerateDepthMapFromLight() override;

private:

	std::shared_ptr<KdModelWork> m_spModel = nullptr;
};