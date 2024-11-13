#include "T_Laser.h"
#include"../../../../../../Scene/SceneManager.h"
#include"../../../../../Bullet/E_B_Laser/E_B_Laser.h"

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
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		if (_spParent->GetSearchFlg())
		{
			TurretBase::UpdateRotate(m_dir);
		}
	}

	Math::Matrix _rotation = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_worldRot.x));

	m_mWorld =  _rotation * m_TransAPMat;

	Math::Vector3 _muzzlePos;
	_muzzlePos = (m_localmuzzleMat * GetMatrix()).Translation();

	if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(_muzzlePos, 0.1f, kRedColor);
	}

	//弾発射
	if (_spParent)
	{
		if (_spParent->GetSearchFlg())
		{
			if (!m_ShotFlg)
			{
				m_Shotcount--;
			}
			if (m_Shotcount < 0)
			{
				m_Shotcount = 0;
			}
			if (m_Shotcount <= 0)
			{
				m_ShotFlg = true;
				m_Shotcount = m_Shotcool;
			}
			//弾発射
			if (m_ShotFlg)
			{
				// レイ判定用パラメーター
				KdCollider::RayInfo _rayInfo;

				//レイの各パラメータを設定
				_rayInfo.m_pos = _muzzlePos;
				_rayInfo.m_dir = m_mWorld.Backward();
				_rayInfo.m_range = 1000.0f;
				_rayInfo.m_type = KdCollider::TypeDamage | KdCollider::TypeGround;

				if (!(GetAsyncKeyState('Q') & 0x8000))
				{
					m_pDebugWire->AddDebugLine(_rayInfo.m_pos, _rayInfo.m_dir, _rayInfo.m_range);
				}

				////衝突情報リスト
				//std::list<KdCollider::CollisionResult> _resultList;

				////作成したレイ情報でオブジェクトリストをと当たり判定
				//for (auto& obj : SceneManager::Instance().GetObjList())
				//{
				//	obj->Intersects(_rayInfo, &_resultList);
				//}

				////レイに当たったリストから一番近いオブジェクトを検出
				//float _maxOverLap = 0.0f;
				//Math::Vector3 _hitPos = Math::Vector3::Zero;
				//bool _ishit = false;
				//for (auto& ret : _resultList)
				//{
				//	//レイが当たった場合の貫通した長さが一番長いものを探す
				//	if (_maxOverLap < ret.m_overlapDistance)
				//	{
				//		_maxOverLap = ret.m_overlapDistance;
				//		_hitPos = ret.m_hitPos;
				//		_ishit = true;
				//	}
				//}

				//２重ループしている 重くなったら改善
				std::list<KdCollider::CollisionResult> _resultList;
				float _maxOverLap = 0.0f;
				Math::Vector3 _hitPos = Math::Vector3::Zero;
				bool _ishit = false;

				//作成したレイ情報でオブジェクトリストをと当たり判定
				for (auto& obj : SceneManager::Instance().GetObjList())
				{
					obj->Intersects(_rayInfo, &_resultList);
					//レイに当たったリストから一番近いオブジェクトを検出
					for (auto& ret : _resultList)
					{
						//レイが当たった場合の貫通した長さが一番長いものを探す
						if (_maxOverLap < ret.m_overlapDistance)
						{
							_maxOverLap = ret.m_overlapDistance;
							_hitPos = ret.m_hitPos;
							if (obj->GetObjectType() == ObjectType::Player)
							{
								_ishit = true;
							}
						}
					}
				}
				//当たっていたら
				if (_ishit)
				{
					//レイの着弾点を利用して弾を飛ばすベクトルを算出
					Math::Vector3 _bulletdir = _hitPos - _muzzlePos;
					_bulletdir.Normalize();

					//発射
					std::shared_ptr<E_B_Laser> _bullet = std::make_shared<E_B_Laser>();
					_bullet->Init();
					_bullet->Shot(_muzzlePos, _bulletdir);
					SceneManager::Instance().AddObject(_bullet);

					//攻撃SE再生
					//KdAudioManager::Instance().Play("Asset/Sounds/P_BulletM.wav", false);
					//弾の発射が終わったらフラグを未発射に戻す
					m_ShotFlg = false;
				}
			}
		}
	}

}

void T_Laser::PostUpdate()
{
}
