#include "ExpDrone.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../Bullet/P_BulletM/P_BulletM.h"

void ExpDrone::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Enemy;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Enemy/ExpDrone/ExpDrone.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("ExpDrone", m_spModel, KdCollider::TypeDamage);
	}
	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void ExpDrone::Update()
{
	if (m_expFlg)
	{
		ExpCollision();
		m_isExpired = true;
		return;
	}

	m_pos = GetPos();
	Math::Matrix _parentMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		m_dir = m_pos - _spTarget->GetPos();
	}

	if (m_ascount > 0)
	{
		//デバッグ
		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			m_pDebugWire->AddDebugSphere(m_pos, m_searchArea, kGreenColor);
		}
		//サーチ判定
		if (m_dir.Length() < m_searchArea)
		{
			m_searchFlg = true;
			m_ascount--;
		}
		else
		{
			m_searchFlg = false;
			m_ascount = m_ascountMax;
		}
		//索敵フラグがONなら
		if (m_searchFlg)
		{
			m_dir.Normalize();
			UpdateRotate(m_dir);
		}
	}
	else
	{
		//突進
		m_pos += GetMatrix().Backward() * m_speed;
	}

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_worldRot.x)) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = Math::Matrix::CreateScale(1.0f) * _rotation * Math::Matrix::CreateTranslation(m_pos);

	// キャラクターの座標が確定してからコリジョンによる位置補正を行う
	UpdateCollision();
}

void ExpDrone::PostUpdate()
{
}

void ExpDrone::OnHit(const int _dmg)
{
	m_hp -= _dmg;
	if (m_hp <= 0)
	{
		m_isExpired = true;
	}
}

void ExpDrone::UpdateRotate(const Math::Vector3& srcMoveVec)
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

	float rotateAng = std::clamp(_betweenAng, -10.0f, 10.0f);
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

	rotateAng = std::clamp(_betweenAng, -10.0f, 10.0f);
	m_worldRot.x += rotateAng;
	//砲塔の射角の制限
	m_worldRot.x = std::clamp(m_worldRot.x, -30.0f, 30.0f);
}

void ExpDrone::UpdateCollision()
{
	{
		//球判定用の変数を作成
		KdCollider::SphereInfo sphereInfo;
		//球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos;
		//球の判定を設定
		sphereInfo.m_sphere.Radius = 3.0f;
		//当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeDamage;

		//デバッグ用
		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		}
		//全オブジェクトと当たり判定!!!!!
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->GetObjectType() == ObjectType::Enemy)continue;
			if (obj->Intersects(sphereInfo, nullptr))
			{
				m_expFlg = true;
			}
		}
	}

	{
		//球判定用の変数を作成
		KdCollider::SphereInfo sphereInfo;
		//球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos;
		//球の判定を設定
		sphereInfo.m_sphere.Radius = 1.0f;
		//当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeGround;

		//デバッグ用
		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		}
		//全オブジェクトと当たり判定!!!!!
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(sphereInfo, nullptr))
			{
				m_expFlg = true;
			}
		}
	}
}

void ExpDrone::ExpCollision()
{

	{
		//球判定用の変数を作成
		KdCollider::SphereInfo sphereInfo;
		//球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos;
		//球の判定を設定
		sphereInfo.m_sphere.Radius = 10.0f;
		//当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeDamage;

		//デバッグ用
		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius, kRedColor);
		}
		//全オブジェクトと当たり判定!!!!!
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(sphereInfo, nullptr))
			{
				//プレイヤーなら当たった判定
				if (obj->GetObjectType() == ObjectType::Player)
				{
					obj->OnHit(m_dmg);
				}
			}
		}
	}
}
