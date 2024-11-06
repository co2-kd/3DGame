#include "T_Leg.h"

#include"../Turret/TurretBase.h"

void T_Leg::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Enemy;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Turret/T_Leg.gltf");
		//デバッグ用
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("T_Leg", m_spModel, KdCollider::TypeDamage);
	}
	if(m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("APTurret");

		//指定ノードが取得出来たら
		if (_pNode)
		{
			m_APTurretMat = _pNode->m_worldTransform;
		}
	}
}

void T_Leg::Update()
{
	m_pos = GetPos();
	Math::Matrix _Mat = Math::Matrix::Identity;
	m_dir = Math::Vector3::Zero;
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		_Mat = Math::Matrix::CreateTranslation(m_localMat.Translation()) * Math::Matrix::CreateTranslation(_spParent->GetMatrix().Translation());
	}
	if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(_Mat.Translation(), 0.1f, kRedColor);
	}

	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		m_dir = m_pos - _spTarget->GetPos();
	}
	// キャラクターの回転行列を創る
	//サーチ判定
	if (m_dir.Length() < m_searchArea)
	{
		m_searchFlg = true;
	}
	else
	{
		m_searchFlg = false;
	}
	m_pDebugWire->AddDebugSphere(m_pos, m_searchArea, kGreenColor);
	if (m_searchFlg)
	{
		UpdateRotate(m_dir);
	}

	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));

	m_mWorld = _rotation * _Mat;
}

void T_Leg::PostUpdate()
{
	//子オブジェクトにAPの座標渡し
	std::shared_ptr<TurretBase> _spTurret = m_wpTurretBase.lock();
	if (_spTurret)
	{
		_spTurret->SetAPMatrix(m_APTurretMat);
	}
}

void T_Leg::UpdateRotate(const Math::Vector3& srcMoveVec)
{

	// 何も入力が無い場合は処理しない
	if (srcMoveVec.LengthSquared() == 0.0f) { return; }

	// キャラの正面方向のベクトル
	Math::Vector3 _nowDir = GetMatrix().Forward();

	// 向きたい方向のベクトル
	Math::Vector3 _targetDir = srcMoveVec;

	_nowDir.Normalize();
	_targetDir.Normalize();

	float _nowAng = atan2(_nowDir.x, _nowDir.z);
	_nowAng = DirectX::XMConvertToDegrees(_nowAng);

	float _targetAng = atan2(_targetDir.x, _targetDir.z);
	_targetAng = DirectX::XMConvertToDegrees(_targetAng);

	// 角度の差分を求める
	float _betweenAng = _targetAng - _nowAng;
	if (_betweenAng > 180)
	{
		_betweenAng -= 360;
	}
	else if (_betweenAng < -180)
	{
		_betweenAng += 360;
	}

	float rotateAng = std::clamp(_betweenAng, -1.0f, 1.0f);
	m_worldRot.y += rotateAng;
	if (m_worldRot.y >= 360)
	{
		m_worldRot.y -= 360;
	}
	else if (m_worldRot.y < 0)
	{
		m_worldRot.y += 360;
	}

}
