#include "CharaBase.h"

void CharaBase::Update()
{
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		//親の行列を取得
		Math::Matrix _parentMat = _spParent->GetMatrix();
		//親の行列を反映したワールド行列を作成
		m_mWorld = m_localMat * _parentMat;
	}
}

void CharaBase::DrawLit()
{
	if (!m_spModel) return;

	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void CharaBase::GenerateDepthMapFromLight()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}
