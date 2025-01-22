#include "Stage1Col.h"

void Stage1Col::Init()
{
	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData("Asset/Models/Ground/Stage1Col.gltf");
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Stage1Col", m_spModel, KdCollider::TypeGround);
	m_mWorld = Math::Matrix::CreateScale(10.0f);

}

void Stage1Col::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Stage1Col::ImguiUpdate()
{
	//ImGui::Text("test : %d", m_mWorld.Translation().x);
}
