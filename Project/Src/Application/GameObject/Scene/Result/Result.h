#pragma once
class Result : public KdGameObject
{
public:
	Result() {}
	~Result()override {}

	void Init()				override;
	void Update()			override;
	void DrawSprite()		override;

private:

	KdTexture m_Result;
	KdTexture m_Resultbar;
	KdTexture m_Resultstart;
	KdTexture m_Resultfade;



	float m_angle = 0.0f;
	float m_angleAdd = 0.01f;
};