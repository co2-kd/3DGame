#include "T_Laser.h"

void T_Laser::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Enemy;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Turret/T_Laser.gltf");
		//デバッグ用
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();
		
	}

	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("APMuzzle");

		//指定ノードが取得出来たら
		if (_pNode)
		{
			m_localmuzzleMat = _pNode->m_worldTransform;
		}
	}
}

void T_Laser::Update()
{
	TurretBase::Update();
	TurretBase::UpdateRotate(m_dir);

	Math::Matrix _rotation = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_worldRot.x));

	m_mWorld = _rotation * m_TransAPMat;

	Math::Vector3 _muzzlePos;
	_muzzlePos = (m_localmuzzleMat * GetMatrix()).Translation();

	if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(_muzzlePos, 0.1f, kRedColor);
	}
}

void T_Laser::PostUpdate()
{
}
