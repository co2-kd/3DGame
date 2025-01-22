#include "Ground.h"

void Ground::Init()
{
	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData("Asset/Models/Ground/Stage1.gltf");
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Stage1", m_spModel, KdCollider::TypeGround);
	m_mWorld = Math::Matrix::CreateScale(10.0f);

}

void Ground::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Ground::ImguiUpdate()
{
	//ImGui::Text("test : %d", m_mWorld.Translation().x);
}
