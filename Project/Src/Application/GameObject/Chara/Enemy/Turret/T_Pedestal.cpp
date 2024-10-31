#include "T_Pedestal.h"

#include"../Turret/T_Leg.h"

void T_Pedestal::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Enemy;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Turret/T_Pedestal.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("T_Pedestal", m_spModel, KdCollider::TypeBump);

		//デバッグ用
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
	}

	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("APLeg");

		//指定ノードが取得出来たら
		if (_pNode)
		{
			m_APLegMat = _pNode->m_worldTransform;
		}
	}
}

void T_Pedestal::Update()
{
	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = Math::Matrix::CreateScale(1.0f) * _rotation * Math::Matrix::CreateTranslation(GetPos());

}

void T_Pedestal::PostUpdate()
{
	//子オブジェクトにAPの座標渡し
	std::shared_ptr<T_Leg> _spLeg = m_wpLeg.lock();
	if (_spLeg)
	{
		_spLeg->SetAPMatrix(m_APLegMat);
	}


}

void T_Pedestal::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void T_Pedestal::OnHit()
{
	
}
