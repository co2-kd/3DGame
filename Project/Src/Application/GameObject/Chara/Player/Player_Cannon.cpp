#include "Player_Cannon.h"
#include"../../../Scene/SceneManager.h"
#include"../../Camera/CameraBase.h"
#include"../../Bullet/P_BulletC/P_BulletC.h"

//初期化
void Player_Cannon::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Player;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Player/Player_Cannon.gltf");


		//初期のアニメ―ション
		m_spAnimator = std::make_shared<KdAnimator>();

	}

	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("APc_muzzle");

		//指定ノードが取得出来たら
		if (_pNode)
		{
			m_localmuzzleMat = _pNode->m_worldTransform;
		}
	}
	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	//初期状態を「待機状態」へ設定
	ChangeActionState(std::make_shared<ActionUnShot>());
}

//更新
void Player_Cannon::Update()
{
	Math::Matrix _parentMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		_parentMat = _spParent->GetMatrix();
	}

	m_muzzlePos = ((m_localmuzzleMat * m_localMat) * _parentMat).Translation();

	////銃口位置をデバッグ表示
	//if (!(GetAsyncKeyState('Q') & 0x8000))
	//{
	//	m_pDebugWire->AddDebugSphere(m_muzzlePos, 0.1f, kRedColor);
	//}

	////デバッグ表示
	//if (!(GetAsyncKeyState('Q') & 0x8000))
	//{
	//	m_pDebugWire->AddDebugSphere(GetMatrix().Translation(), 0.1f, kWhiteColor);
	//}

	//現在の状態の更新呼び出し
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}

	CharaBase::Update();

}

//後更新
void Player_Cannon::PostUpdate()
{

	//アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}


//imguiの更新
void Player_Cannon::ImguiUpdate()
{

}

void Player_Cannon::ShotBullet()
{
	m_shotFlg = true;
}

//カメラから見た移動方向に向く処理
void Player_Cannon::UpdateRotate(const Math::Vector3& srcMoveVec)
{
	// 何も入力が無い場合は処理しない
	if (srcMoveVec.LengthSquared() == 0.0f/* || (waitdrivetime >= 1 && GetAsyncKeyState(VK_CONTROL))*/) { return; }

	// キャラの正面方向のベクトル
	Math::Vector3 _nowDir = GetMatrix().Backward();

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
}


//---------------------------------------------------------------------------------------
//ここから下はステートパターン関係
//---------------------------------------------------------------------------------------
void Player_Cannon::ChangeActionState(std::shared_ptr<ActionStateBase> nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextState;
	m_nowAction->Enter(*this);
}

void Player_Cannon::ActionShoting::Enter(Player_Cannon& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Shot"), false);
}

void Player_Cannon::ActionShoting::Update(Player_Cannon& owner)
{

	if (owner.m_shotFlg)
	{
		//弾発射
		Math::Vector3 _cameraPos;
		Math::Vector3 _dir;
		float _range = 0.0f;
		if (owner.m_wpCamera.expired() == false)
		{
			//レティクル方向に弾発射
			_cameraPos = owner.m_wpCamera.lock()->GetPos();
			//カメラの向き情報の作成
			owner.m_wpCamera.lock()->WorkCamera()->GenerateRayInfoFromClientPos({ 640,360 }, _cameraPos, _dir, _range);
		}
		// レイ判定用パラメーター
		KdCollider::RayInfo _rayInfo;

		//レイの各パラメータを設定
		_rayInfo.m_pos = _cameraPos;
		//_rayInfo.m_pos = _muzzlePos;
		_rayInfo.m_dir = _dir;
		//_rayInfo.m_dir = _parentMat.Backward();
		_rayInfo.m_range = _range;
		//_rayInfo.m_range = 10000;
		_rayInfo.m_type = KdCollider::TypeGround | KdCollider::TypeBump;

		//if (!(GetAsyncKeyState('Q') & 0x8000))
		//{
		//	owner.m_pDebugWire->AddDebugLine(_rayInfo.m_pos, _rayInfo.m_dir, _rayInfo.m_range);
		//}

		//衝突情報リスト
		std::list<KdCollider::CollisionResult> _resultList;

		//作成したレイ情報でオブジェクトリストをと当たり判定
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			obj->Intersects(_rayInfo, &_resultList);
		}

		//レイに当たったリストから一番近いオブジェクトを検出
		bool _ishit = false;
		float _maxOverLap = 0.0f;
		Math::Vector3 _hitPos = Math::Vector3::Zero;
		for (auto& ret : _resultList)
		{
			//レイが当たった場合の貫通した長さが一番長いものを探す
			if (_maxOverLap < ret.m_overlapDistance)
			{
				_maxOverLap = ret.m_overlapDistance;
				_hitPos = ret.m_hitPos;
				_ishit = true;
			}
		}
		//当たっていたら
		if (_ishit)
		{
			//レイの着弾点を利用して弾を飛ばすベクトルを算出
			Math::Vector3 _bulletdir = _hitPos - owner.m_muzzlePos;
			_bulletdir.Normalize();

			//発射
			std::shared_ptr<P_BulletC> _bullet = std::make_shared<P_BulletC>();
			_bullet->Init();
			_bullet->Shot(owner.m_muzzlePos, _bulletdir);
			SceneManager::Instance().AddObject(_bullet);

			//攻撃SE再生
			//KdAudioManager::Instance().Play("Asset/Sounds/P_BulletC.wav", false);
		}
		else
		{
			//レイの着弾点を利用して弾を飛ばすベクトルを算出
			Math::Vector3 _bulletdir = (_rayInfo.m_dir * _rayInfo.m_range) - owner.m_muzzlePos;
			_bulletdir.Normalize();
			//発射
			std::shared_ptr<P_BulletC> _bullet = std::make_shared<P_BulletC>();
			_bullet->Init();
			_bullet->Shot(owner.m_muzzlePos, _bulletdir);
			SceneManager::Instance().AddObject(_bullet);

			//攻撃SE再生
			//KdAudioManager::Instance().Play("Asset/Sounds/P_BulletC.wav", false);
		}
		//弾の発射が終わったらフラグを未発射に戻す
		owner.m_shotFlg = false;
	}

	if (owner.m_spAnimator->IsAnimationEnd())
	{
		owner.ChangeActionState(std::make_shared<ActionUnShot>());
	}
}

void Player_Cannon::ActionShoting::Exit(Player_Cannon& owner)
{
	owner.m_shotFlg = false;
}

void Player_Cannon::ActionUnShot::Enter(Player_Cannon& owner)
{
}

void Player_Cannon::ActionUnShot::Update(Player_Cannon& owner)
{
	if (owner.m_shotFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionShoting>());
	}
}

void Player_Cannon::ActionUnShot::Exit(Player_Cannon& owner)
{
}
