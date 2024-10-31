#pragma once
class Title : public KdGameObject
{
public:
	Title() {}
	~Title()override {}

	void Init()				override;
	void Update()			override;
	void DrawSprite()		override;

private:

	KdTexture m_Title;
	KdTexture m_Titlebar;
	KdTexture m_Titlestart;



	float m_angle = 0.0f;
	float m_angleAdd = 0.01f;
};