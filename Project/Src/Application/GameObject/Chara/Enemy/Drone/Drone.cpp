#include "Drone.h"

#include"../../../Bullet/P_BulletM/P_BulletM.h"

void Drone::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Enemy;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Drone/Drone2.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Drone", m_spModel, KdCollider::TypeDamage);

		//if (m_spModel)
		//{
		//	//blenderで作成したNULLポイントノードを探して取得
		//	const KdModelWork::Node* _pNode = m_spModel->FindNode("APbattery");

		//	if (m_spModel)
		//	{
		//		//blenderで作成したNULLポイントノードを探して取得
		//		const KdModelWork::Node* _pNode = m_spModel->FindNode("APm_muzzle");

		//		//指定ノードが取得出来たら
		//		if (_pNode)
		//		{
		//			m_localmuzzleMat = _pNode->m_worldTransform;
		//		}
		//	}

		//}
	}
}

void Drone::Update()
{
	m_pos = GetPos();
	Math::Matrix _parentMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		m_dir = m_pos - _spTarget->GetPos();
	}

	UpdateRotate(m_dir);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = Math::Matrix::CreateScale(2.0f) * _rotation * Math::Matrix::CreateTranslation(m_pos);
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
	m_hp--;

	if (m_hp <= 0)
	{
		m_isExpired = true;
	}
}

void Drone::UpdateRotate(const Math::Vector3& srcMoveVec)
{
	// 何も入力が無い場合は処理しない
	if (srcMoveVec.LengthSquared() == 0.0f/* || (waitdrivetime >= 1 && GetAsyncKeyState(VK_CONTROL))*/) { return; }

	// キャラの正面方向のベクトル
	Math::Vector3 _nowDir = GetMatrix().Forward();

	// 移動方向のベクトル
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