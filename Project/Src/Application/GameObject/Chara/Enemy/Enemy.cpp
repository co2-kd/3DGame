#include "Enemy.h"

void Enemy::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Enemy2.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Enemy", m_spModel, KdCollider::TypeDamage);

		m_mWorld = Math::Matrix::CreateScale(5.0f) * Math::Matrix::CreateTranslation(0,-10,200);
	}
}

void Enemy::Update()
{

}

void Enemy::PostUpdate()
{
}

void Enemy::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Enemy::GenerateDepthMapFromLight()
{
}
