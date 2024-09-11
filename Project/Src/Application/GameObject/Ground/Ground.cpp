#include "Ground.h"

void Ground::Init()
{
	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData("Asset/Models/Ground/ground.gltf");
	Math::Matrix scaleMat = Math::Matrix::CreateScale(100);
	m_mWorld = scaleMat;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("StageCollison", m_spModel, KdCollider::TypeGround);
}

void Ground::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Ground::ImguiUpdate()
{
	ImGui::Text("test : %d", m_mWorld.Translation().x);
}
