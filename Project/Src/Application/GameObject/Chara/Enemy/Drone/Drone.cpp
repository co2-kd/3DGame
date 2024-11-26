#include "Drone.h"

#include"../../../Bullet/P_BulletM/P_BulletM.h"

void Drone::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Enemy;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/Drone/Drone.gltf");
		m_spLightModel = std::make_shared<KdModelWork>();
		m_spLightModel->SetModelData("Asset/Models/Enemy/Drone/DroneLight.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Drone", m_spModel, KdCollider::TypeDamage);

		if (m_spModel)
		{
			//blenderで作成したNULLポイントノードを探して取得
			const KdModelWork::Node* _pNode = m_spModel->FindNode("APmuzzle");

			//指定ノードが取得出来たら
			if (_pNode)
			{
				m_localmuzzleMat = _pNode->m_worldTransform;
			}
		}
	}

	//初期状態を「待機状態」へ設定
	ChangeActionState(std::make_shared<ActionSearch>());

	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
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

	//現在の状態の更新呼び出し
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}

	m_dir.Normalize();
	UpdateRotate(m_dir);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_worldRot.x)) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = Math::Matrix::CreateScale(2.0f) * _rotation * Math::Matrix::CreateTranslation(m_pos);

	Math::Vector3 _muzzlePos;
	_muzzlePos = (m_localmuzzleMat * GetMatrix()).Translation();

	if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(_muzzlePos, 0.1f, kRedColor);
	}
}

void Drone::PostUpdate()
{
}

void Drone::DrawBright()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spLightModel, m_mWorld);
}

void Drone::OnHit(const int _dmg)
{
	m_hp -= _dmg;
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


	_nowAng = atan(_nowDir.y);
	_nowAng = DirectX::XMConvertToDegrees(_nowAng);

	_targetAng = atan(_targetDir.y);
	_targetAng = DirectX::XMConvertToDegrees(_targetAng);

	// 角度の差分を求める
	_betweenAng = _targetAng - _nowAng;
	if (_betweenAng > 180)
	{
		_betweenAng -= 360;
	}
	else if (_betweenAng < -180)
	{
		_betweenAng += 360;
	}

	rotateAng = std::clamp(_betweenAng, -1.0f, 1.0f);
	m_worldRot.x += rotateAng;
	//砲塔の射角の制限
	m_worldRot.x = std::clamp(m_worldRot.x, -30.0f, 30.0f);
}

void Drone::ChangeActionState(std::shared_ptr<ActionStateBase> nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextState;
	m_nowAction->Enter(*this);
}

void Drone::ActionSearch::Enter(Drone& owner)
{
}

void Drone::ActionSearch::Update(Drone& owner)
{
}

void Drone::ActionSearch::Exit(Drone& owner)
{
}

void Drone::ActionBattle::Enter(Drone& owner)
{
}

void Drone::ActionBattle::Update(Drone& owner)
{
}

void Drone::ActionBattle::Exit(Drone& owner)
{
}
