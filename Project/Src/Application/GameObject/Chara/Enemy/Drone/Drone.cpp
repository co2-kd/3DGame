#include "Drone.h"

void Drone::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Enemy;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Drone/Drone2.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Drone", m_spModel, KdCollider::TypeDamage);

		m_mWorld = /*Math::Matrix::CreateScale(1.0f) **/ Math::Matrix::CreateTranslation(0, 10, 200);
	}
}

void Drone::Update()
{ 
}

void Drone::PostUpdate()
{
}

void Drone::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Drone::OnHit()
{
	m_isExpired = true;
}
