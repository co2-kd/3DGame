#include "Drone.h"
#include"../../../../Scene/SceneManager.h"
#include"../../../Bullet/E_D_Bullet/E_D_Bullet.h"

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

	m_startPos = m_mWorld.Translation();

	//初期状態を「待機状態」へ設定
	ChangeActionState(std::make_shared<ActionSearch>());

	//乱数生成
	m_RandomGen = std::make_shared<KdRandomGenerator>();

	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void Drone::Update()
{
	m_pos = GetPos();

	//重力
	m_pos.y -= m_gravity - m_floating;
	//重力加算
	if (m_floating > m_floatingMin)
	{
		m_floating -= m_floatingWeak;
	}

	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		m_dir = m_pos - _spTarget->GetPos();
		m_length = m_dir.Length();
	}

	//現在の状態の更新呼び出し
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}
	/*if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(m_pos, m_searchArea, kGreenColor);
	}*/

	//移動
	m_pos += m_moveVec * m_speed;

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationX(DirectX::XMConvertToRadians(m_worldRot.x)) * Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = Math::Matrix::CreateScale(2.0f) * _rotation * Math::Matrix::CreateTranslation(m_pos);

	// 座標が確定してからコリジョンによる位置補正を行う
	UpdateCollision();

	//弾発射
	if (m_ShotFlg)
	{
		ShotBullet();
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

void Drone::UpdateCollision()
{	
	// 地面判定

	static const float enableStepHigh = m_adjustHeight + 2.0f;

	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高い所から飛ばす(段差の許容範囲)
	//rayInfo.m_pos.y += 1.0f;
	rayInfo.m_pos.y = rayInfo.m_pos.y - m_adjustHeight + enableStepHigh;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さ
	rayInfo.m_range = m_gravity - m_floating + enableStepHigh;
	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeBump | KdCollider::TypeGround;
	/*if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);
	}*/

	// ②HIT判定対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);
			// ③結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			float maxOverLap = 0;
			Math::Vector3 hitPos = Math::Vector3::Zero;
			bool hit = false;
			for (auto& ret : retRayList)
			{
				// レイを遮断しオーバーした長さが
				// 一番長いものを探す
				if (maxOverLap < ret.m_overlapDistance)
				{
					maxOverLap = ret.m_overlapDistance;
					hitPos = ret.m_hitPos;
					hit = true;
				}
			}
			// 何かしらの上に乗ってる
			if (hit)
			{
				//浮力
				//hitPos.y += m_adjustHeight;
				//SetPos(hitPos);
				m_floating += maxOverLap;
			}
		}
	}

	// その他球による衝突判定
	// ---- ---- ---- ---- ---- ----
	// ①当たり判定(球判定)用の情報を作成
	KdCollider::SphereInfo sphereInfo;
	sphereInfo.m_sphere.Center = GetPos() + Math::Vector3(0, 2.0f, 0);
	sphereInfo.m_sphere.Radius = 2.5f;
	sphereInfo.m_type = KdCollider::TypeGround | KdCollider::TypeBump;

	//if (!(GetAsyncKeyState('Q') & 0x8000))
	//{
	//	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius, kRedColor);
	//}

	// ②HIT対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> weGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = weGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retBumpList;
			spGameObj->Intersects(sphereInfo, &retBumpList);

			// ③結果を使って座標を補完する
			for (auto& ret : retBumpList)
			{
				Math::Vector3 newPos = GetPos() + (ret.m_hitDir * ret.m_overlapDistance);
				SetPos(newPos);
			}
		}
	}
}

void Drone::ShotBullet()
{
	//弾発射
	Math::Vector3 _muzzlePos;
	_muzzlePos = (m_localmuzzleMat * GetMatrix()).Translation();

	/*if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(_muzzlePos, 0.1f, kRedColor);
	}*/
	// レイ判定用パラメーター
	KdCollider::RayInfo _rayInfo;

	//レイの各パラメータを設定
	_rayInfo.m_pos = _muzzlePos;
	_rayInfo.m_dir = m_mWorld.Backward();
	_rayInfo.m_range = m_length;
	_rayInfo.m_type = KdCollider::TypeDamage | KdCollider::TypeGround;

	/*if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugLine(_rayInfo.m_pos, _rayInfo.m_dir, _rayInfo.m_range);
	}*/

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
		std::shared_ptr<E_D_Bullet> _bullet = std::make_shared<E_D_Bullet>();
		_bullet->Init();
		_bullet->Shot(_muzzlePos, _bulletdir);
		SceneManager::Instance().AddObject(_bullet);

		//攻撃SE再生
		//KdAudioManager::Instance().Play("Asset/Sounds/P_BulletM.wav", false);
		//弾の発射が終わったらフラグを未発射に戻す
		m_ShotFlg = false;
	}
}

void Drone::ChangeActionState(std::shared_ptr<ActionStateBase> nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextState;
	m_nowAction->Enter(*this);
}

void Drone::ChangeBattleState(std::shared_ptr<BattleStateBase> nextState)
{
	if (m_nowRange)m_nowRange->Exit(*this);
	m_nowRange = nextState;
	m_nowRange->Enter(*this);
}

void Drone::ActionSearch::Enter(Drone& owner)
{
	//索敵範囲を代入
	owner.m_searchArea = owner.m_searchAreaSearch;
}

void Drone::ActionSearch::Update(Drone& owner)
{
	owner.m_moveVec = Math::Vector3::Zero;

	//
	owner.UpdateRotate(Math::Vector3::Backward);

	//サーチ判定
	if (owner.m_length < owner.m_searchArea)
	{
		//状態を「交戦状態」へ設定
		owner.ChangeActionState(std::make_shared<ActionBattle>());
	}
}

void Drone::ActionSearch::Exit(Drone& owner)
{
}

void Drone::ActionBattle::Enter(Drone& owner)
{
	owner.m_searchArea = owner.m_searchAreaBattle;
	owner.m_keepcount = 0;
}

void Drone::ActionBattle::Update(Drone& owner)
{
	owner.m_keepcount--;
	if (owner.m_keepcount <= 0)
	{
		owner.m_keepcount = owner.m_keepcountMax;
		owner.m_moveVec = Math::Vector3::Zero;
		//乱数用
		if (owner.m_length <= owner.m_keepAreaMax && owner.m_length >= owner.m_keepAreaMin)
		{
			owner.m_rnd = owner.m_RandomGen->GetInt(0, owner.m_bunbo - 1);
			owner.ChangeBattleState(std::make_shared<RangeJust>());
		}
		else if (owner.m_length > owner.m_keepAreaMax)
		{
			owner.ChangeBattleState(std::make_shared<RangeFar>());
		}
		else if (owner.m_length < owner.m_keepAreaMin)
		{
			owner.ChangeBattleState(std::make_shared<RangeNear>());
		}
	}

	//距離ごとの更新呼び出し
	if (owner.m_nowRange)
	{
		owner.m_nowRange->Update(owner);
	}

	//サーチ判定
	if (owner.m_length < owner.m_searchArea)
	{

		owner.m_dir.Normalize();
		owner.UpdateRotate(owner.m_dir);
	}
	else
	{
		//状態を「帰還状態」へ設定
		owner.ChangeActionState(std::make_shared<ActionReturn>());
	}
}

void Drone::ActionBattle::Exit(Drone& owner)
{

}

void Drone::ActionReturn::Enter(Drone& owner)
{
	//索敵範囲を代入
	owner.m_searchArea = owner.m_searchAreaSearch;
}

void Drone::ActionReturn::Update(Drone& owner)
{
	//帰還方向を決める
	owner.m_dir = owner.m_pos - owner.m_startPos;
	owner.m_length = owner.m_dir.Length();

	//移動
	owner.m_moveVec = Math::Vector3::Zero;
	owner.m_moveVec = owner.GetMatrix().Backward();

	//帰還方向を向く
	owner.m_dir.Normalize();
	owner.UpdateRotate(owner.m_dir);

	//初期座標に戻ったら索敵状態に戻す
	if (owner.m_length < 1.0f)
	{
		owner.ChangeActionState(std::make_shared<ActionSearch>());
	}

}

void Drone::ActionReturn::Exit(Drone& owner)
{
}

void Drone::RangeJust::Enter(Drone& owner)
{
}

void Drone::RangeJust::Update(Drone& owner)
{
	if (owner.m_rnd < 300)
	{
		owner.m_moveVec = owner.GetMatrix().Left();
	}
	else if(owner.m_rnd < 600)
	{
		owner.m_moveVec = owner.GetMatrix().Right();
	}
	else
	{
		owner.m_moveVec = Math::Vector3::Zero;
	}

	if (!owner.m_ShotFlg)
	{
		owner.m_Shotcount--;
		if (owner.m_Shotcount < 0)
		{
			owner.m_Shotcount = 0;
		}
	}
	if (owner.m_Shotcount <= 0)
	{
		owner.m_ShotFlg = true;
		owner.m_Shotcount = owner.m_Shotcool;
	}
}

void Drone::RangeJust::Exit(Drone& owner)
{
}

void Drone::RangeFar::Enter(Drone& owner)
{
}

void Drone::RangeFar::Update(Drone& owner)
{
	owner.m_moveVec = owner.GetMatrix().Backward();
}

void Drone::RangeFar::Exit(Drone& owner)
{
}

void Drone::RangeNear::Enter(Drone& owner)
{
}

void Drone::RangeNear::Update(Drone& owner)
{
	owner.m_moveVec = owner.GetMatrix().Forward();

	if (!owner.m_ShotFlg)
	{
		owner.m_Shotcount--;
		if (owner.m_Shotcount < 0)
		{
			owner.m_Shotcount = 0;
		}
	}
	if (owner.m_Shotcount <= 0)
	{
		owner.m_ShotFlg = true;
		owner.m_Shotcount = owner.m_Shotcool;
	}
}

void Drone::RangeNear::Exit(Drone& owner)
{
}
