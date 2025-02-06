#include "Result.h"

void Result::Init()
{
	m_Result.Load("Asset/Textures/Title.png");
	m_Resultbar.Load("Asset/Textures/Resultbar.png");
	m_Resultstart.Load("Asset/Textures/Resultend.png");
	m_Resultfade.Load("Asset/Textures/fade.png");

	m_angle = 0.5f;
}

void Result::Update()
{
	m_angle += m_angleAdd;

	if (m_angle >= 1 || m_angle <= 0.3f) {
		m_angleAdd *= -1;
	}
}

void Result::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Result, 0, 0, 1280, 720);
	Math::Color color2 = { 1,1,1,0.7f };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Resultfade, 0, 0,nullptr,&color2);
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Resultbar, 0, 0, 1280, 720);
	Math::Color color = { 1, 1, 1,m_angle };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_Resultstart, 0, -200,nullptr,&color);
}
