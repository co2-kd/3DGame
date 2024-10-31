#include "Title.h"

void Title::Init()
{
	m_Title.Load("Asset/Textures/Title.png");
	m_Titlebar.Load("Asset/Textures/Titlebar.png");
	m_Titlestart.Load("Asset/Textures/Titlestart.png");

	m_angle = 0.5f;
}

void Title::Update()
{
	m_angle += m_angleAdd;

	if (m_angle >= 1 || m_angle <= 0.3f) {
		m_angleAdd *= -1;
	}
}

void Title::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Title, 0, 0, 1280, 720);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Titlebar, 0, 200, 960, 270);
	Math::Color color = { 1, 1, 1,m_angle };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Titlestart, 0, -200,nullptr,&color);
}
