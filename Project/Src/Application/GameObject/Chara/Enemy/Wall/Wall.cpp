#include "Wall.h"

void Wall::Init()
{
	m_objectType = ObjectType::Enemy;
	m_spModel = std::make_shared<KdModelWork>();
	m_spModel->SetModelData("Asset/Models/Enemy/Wall/Wall.gltf");
	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("Wall", m_spModel, KdCollider::TypeGround);

	Math::Matrix m_transMat = Math::Matrix::CreateTranslation(m_mWorld.Translation());
	m_mWorld = Math::Matrix::CreateScale(10.0f) * m_transMat;

}

void Wall::Update()
{
}

void Wall::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Wall::OnHit(const int _dmg)
{
	m_hp -= _dmg;
	if (m_hp <= 0)
	{
		m_destFlg = true;
		m_isExpired = true;
	}
}

void Wall::ImguiUpdate()
{
	//ImGui::Text("test : %d", m_mWorld.Translation().x);
}
