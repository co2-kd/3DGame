#include "Back.h"

void Back::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Back/Back.gltf");

		m_mWorld = Math::Matrix::CreateScale(100.0f) * Math::Matrix::CreateTranslation(0,0,0);
	}
}

void Back::Update()
{

}

void Back::PostUpdate()
{
}

void Back::DrawUnLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
