#include "Player_Minigun.h"
#include"../../../Scene/SceneManager.h"
#include"Application/main.h"
#include"../../Camera/CameraBase.h"
#include"../../Bullet/P_BulletM/P_BulletM.h"

//初期化
void Player_Minigun::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Player;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Player/Player_Minigun.gltf");


		//初期のアニメ―ション
		m_spAnimator = std::make_shared<KdAnimator>();
		

	}


	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("APm_muzzle");

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

void Player_Minigun::PreUpdate()
{
	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto efklist = KdEffekseerManager::GetInstance().GetnowEffectPlayList();
	auto efkit = efklist.begin();

	while (efkit != efklist.end())
	{
		if (!(*efkit)->IsPlaying())	// IsPlaying() ・・・ハンドルが0( 未再生 or 再生終了 )でない場合はTrue, 
		{
			auto it = m_efkList.begin();
			// 無効なエフェクトをリストから削除
			while (it != m_efkList.end())
			{
				if ((*it)->GetHandle() == (*efkit)->GetHandle())
				{
					//it = m_efkList.erase(it);
					break;
				}
				else
				{
					++it;	// 次の要素へイテレータを進める
				}
			}
			// 無効なエフェクトをリストから削除
			efkit = efklist.erase(efkit);

		}
		else
		{
			auto it = m_efkList.begin();
			while (it != m_efkList.end())
			{
				if ((*it)->GetHandle() == (*efkit)->GetHandle())
				{
					KdEffekseerManager::GetInstance().SetPos((*it)->GetHandle(), m_muzzlePos);
					(*it)->SetPos(m_muzzlePos);
					
				}
				++it;
			}
			++efkit;	// 次の要素へイテレータを進める
		}
	}
	//m_efkList = KdEffekseerManager::GetInstance().GetnowEffectPlayList();

}

//更新
void Player_Minigun::Update()
{
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		const KdModelWork::Node* _pNode = _spParent->GetModel()->FindNode("APminigun");
		if (_pNode)
		{
			m_localMat = _pNode->m_worldTransform;
		}
		m_worldRot = _spParent->GetWorldRot();
	}

	//座標を確定
	CharaBase::Update();

	//銃口の座標を作成
	Math::Matrix _muzzleMat = m_localmuzzleMat * GetMatrix();
	m_muzzlePos = _muzzleMat.Translation();
	//銃口位置をデバッグ表示
	if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(m_muzzlePos, 0.1f, kRedColor);
	}

	//現在の状態の更新呼び出し
	if (m_nowAction)
	{
		m_nowAction->Update(*this);
	}


	//for (auto& efk : m_efkList)
	//{
	//	KdEffekseerManager::GetInstance().SetPos(efk->GetHandle(), m_muzzlePos);
	//}

	//m_dir = m_mWorld.Translation() - m_pos;
	//m_dir.Normalize();
	//m_pos = GetPos();

}

//後更新
void Player_Minigun::PostUpdate()
{

	//アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

////モデル描画
//void Player_Minigun::DrawLit()
//{
//}
//
////影描画
//void Player_Minigun::GenerateDepthMapFromLight()
//{
//	if (!m_spModel) return;
//
//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
//}

//imguiの更新
void Player_Minigun::ImguiUpdate()
{
	ImGui::Text("m_muzzlePosx : %.3f", m_muzzlePos.x);
	ImGui::Text("m_muzzlePosy: %.3f", m_muzzlePos.y);
	ImGui::Text("m_muzzlePosz : %.3f", m_muzzlePos.z);
	auto it = m_efkList.begin();
	// 無効なオブジェクトをリストから削除
	while (it != m_efkList.end())
	{
		Application::Instance().m_log.AddLog("m_efkListh%d\n",(*it)->GetHandle());
		Application::Instance().m_log.AddLog("m_efkListh%f\n",(*it)->GetPos().x);
		++it;
	}
	ImGui::Text("m_efkList%d\n", m_efkList.size());
	ImGui::Text("efklist : %d", KdEffekseerManager::GetInstance().GetnowEffectPlayList().size());

}

void Player_Minigun::ShotBullet()
{
	//発射フラグON
	m_shotFlg = true;
}

//カメラから見た移動方向に向く処理
void Player_Minigun::UpdateRotate(const Math::Vector3& srcMoveVec)
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

////当たり判定
//void Player_Minigun::UpdateCollision()
//{
//
//	// ①当たり判定(レイ判定)用の情報作成
//	KdCollider::RayInfo rayInfo;
//	// レイの発射位置を設定
//	rayInfo.m_pos = GetPos();
//	// 少し高い所から飛ばす(段差の許容範囲)
//	rayInfo.m_pos.y += 0.4f;
//
//	// レイの発射方向を設定
//	rayInfo.m_dir = Math::Vector3::Down;
//
//	// レイの長さ
//	rayInfo.m_range = 0.2f;
//
//	// 当たり判定をしたいタイプを設定
//	rayInfo.m_type = KdCollider::TypeGround;
//
//	m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);
//}

//---------------------------------------------------------------------------------------
//ここから下はステートパターン関係
//---------------------------------------------------------------------------------------
void Player_Minigun::ChangeActionState(std::shared_ptr<ActionStateBase> nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextState;
	m_nowAction->Enter(*this);
}

void Player_Minigun::ActionUnShot::Enter(Player_Minigun& owner)
{
	
}

void Player_Minigun::ActionUnShot::Update(Player_Minigun& owner)
{
	if (owner.m_shotFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionShotBegin>());
	}
}

void Player_Minigun::ActionUnShot::Exit(Player_Minigun& owner)
{

}


void Player_Minigun::ActionShotBegin::Enter(Player_Minigun& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Shotbegin"), false);
}

void Player_Minigun::ActionShotBegin::Update(Player_Minigun& owner)
{
	if (owner.m_spAnimator->IsAnimationEnd())
	{
		owner.ChangeActionState(std::make_shared<ActionShoting>());
	}
}

void Player_Minigun::ActionShotBegin::Exit(Player_Minigun& owner)
{
}

void Player_Minigun::ActionShoting::Enter(Player_Minigun& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Shoting"), true);
}

void Player_Minigun::ActionShoting::Update(Player_Minigun& owner)
{
	if (!(GetAsyncKeyState(VK_RBUTTON)& 0x8000))
	{
		owner.ChangeActionState(std::make_shared<ActionShotEnd>());
	}

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
		//_rayInfo.m_pos = owner.m_muzzlePos;
		_rayInfo.m_dir = _dir;
		//_rayInfo.m_dir = _parentMat.Backward();
		_rayInfo.m_range = _range;
		//_rayInfo.m_range = 10000;
		_rayInfo.m_type = KdCollider::TypeGround | KdCollider::TypeDamage | KdCollider::TypeBump;

		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			owner.m_pDebugWire->AddDebugLine(_rayInfo.m_pos, _rayInfo.m_dir, _rayInfo.m_range);
		}

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
			std::shared_ptr<P_BulletM> _bullet = std::make_shared<P_BulletM>();
			_bullet->Init();
			_bullet->Shot(owner.m_muzzlePos, _bulletdir);
			SceneManager::Instance().AddObject(_bullet);

			//攻撃SE再生
			KdAudioManager::Instance().Play("Asset/Sounds/P_BulletM.wav", false);
			//エフェクト再生
			auto _spEffect = KdEffekseerManager::GetInstance().Play("muzzleflash/muzzleflash.efk", owner.m_muzzlePos, owner.m_worldRot, 1, 1.5, false).lock();
			KdEffekseerManager::GetInstance().Play("muzzleflash/muzzleflash.efk", owner.m_muzzlePos, owner.m_worldRot, 1, 1.5, false);
			if (_spEffect)
			{
				owner.AddEffect(_spEffect);
			}
		}
		else
		{
			//レイの着弾点を利用して弾を飛ばすベクトルを算出
			Math::Vector3 _bulletdir = (_rayInfo.m_dir * _rayInfo.m_range);
			_bulletdir.Normalize();
			//発射
			std::shared_ptr<P_BulletM> _bullet = std::make_shared<P_BulletM>();
			_bullet->Init();
			_bullet->Shot(owner.m_muzzlePos, _bulletdir);
			SceneManager::Instance().AddObject(_bullet);

			//攻撃SE再生 
			KdAudioManager::Instance().Play("Asset/Sounds/P_BulletM.wav", false);
			KdEffekseerManager::GetInstance().Play("muzzleflash/muzzleflash.efk", owner.m_muzzlePos, owner.m_worldRot, 1, 1.5, false);
		}
		//弾の発射が終わったらフラグを未発射に戻す
		owner.m_shotFlg = false;
	}
}

void Player_Minigun::ActionShoting::Exit(Player_Minigun& owner)
{
	owner.m_shotFlg = false;
}

void Player_Minigun::ActionShotEnd::Enter(Player_Minigun& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Shotend"), false);
}

void Player_Minigun::ActionShotEnd::Update(Player_Minigun& owner)
{
	if (owner.m_shotFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionShotBegin>());
	}

	if (owner.m_spAnimator->IsAnimationEnd())
	{
		owner.ChangeActionState(std::make_shared<ActionUnShot>());
	}
}

void Player_Minigun::ActionShotEnd::Exit(Player_Minigun& owner)
{
}
