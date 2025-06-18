#include "Player.h"
#include "../../../main.h"
#include"../../Camera/CameraBase.h"

#include"../Player/Player_Battery.h"


//初期化
void Player::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::Player;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Player/Player_main.gltf");

		m_pCollider = std::make_unique<KdCollider>();
		m_pCollider->RegisterCollisionShape("Player", m_spModel, KdCollider::TypeDamage);
		//デバッグ用
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		//初期のアニメ―ション
		m_spAnimator = std::make_shared<KdAnimator>();

	}
	m_pos.y = 10.0f;
	SetPos(m_pos);
	//「AttachPoint」ノードを取得する
	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("APbattery");

		m_APBatteryMat = _pNode->m_worldTransform;

	}

	//子オブジェクトにAPの座標渡し
	std::shared_ptr<Player_Battery> _spBattery = m_wpBattery.lock();
	if (_spBattery)
	{
		_spBattery->SetAPMatrix(m_APBatteryMat);
	}

	m_barbackTex.Load("Asset/Textures/bar_back.png");
	m_barflontTex.Load("Asset/Textures/bar_flont.png");
	m_boostTex.Load("Asset/Textures/boost_bar.png");
	m_hpTex.Load("Asset/Textures/hp_bar.png");
	m_reticleTex.Load("Asset/Textures/cross.png");

	m_boostefk = KdEffekseerManager::GetInstance().Play("boost/boost.efk", m_pos, m_worldRot, 0, 10, false).lock();
	m_jumpefk = KdEffekseerManager::GetInstance().Play("jump/jump.efk", m_pos, m_worldRot, 0, 100, false).lock();

	//初期状態を「待機状態」へ設定
	ChangeActionState(std::make_shared<ActionIdle>());

}
void Player::PreUpdate()
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
					KdEffekseerManager::GetInstance().SetWorldMatrix((*it)->GetHandle(), m_mWorld);
				}
				//ダッシュ終了時エフェクト停止、追加
				if (m_boostefk->GetHandle() == (*efkit)->GetHandle())
				{
					if (m_boostendefkFlg)
					{
						KdEffekseerManager::GetInstance().StopEffect(m_boostefk->GetHandle());
					}
				}
				//ジャンプ終了時エフェクト停止、追加
				if (m_jumpefk->GetHandle() == (*efkit)->GetHandle())
				{
					if (!m_jumpFlg || m_overheatFlg)
					{
						KdEffekseerManager::GetInstance().StopEffect(m_jumpefk->GetHandle());
					}
				}
				++it;
			}
			++efkit;	// 次の要素へイテレータを進める
		}
	}
	//m_efkList = KdEffekseerManager::GetInstance().GetnowEffectPlayList();

}

//更新
void Player::Update()
{
	m_pos = GetPos();
	m_boostFlg = false;
	
	//重力
	m_pos.y -= m_gravity - m_floating;
	//重力加算
	if (m_floating > m_floatingMin)
	{
		m_floating -= m_floatingWeak;
	}
	m_moveVec = Math::Vector3::Zero;

	//現在の状態の更新呼び出し
	if (m_nowAction)
	{
		//移動ベクトル
		if (GetAsyncKeyState('D')) { m_moveVec.x += 1.0f; }
		if (GetAsyncKeyState('A')) { m_moveVec.x -= 1.0f; }
		if (GetAsyncKeyState('W')) { m_moveVec.z += 1.0f; }
		if (GetAsyncKeyState('S')) { m_moveVec.z -= 1.0f; }
		if (m_moveVec.LengthSquared() == 0.0f) { m_speed = 0.0f; }
		m_nowAction->Update(*this);
	}
	//カメラ取得
	const std::shared_ptr<const CameraBase> _spCamera = m_wpCamera.lock();
	if (_spCamera)
	{
		m_moveVec = m_moveVec.TransformNormal(m_moveVec, _spCamera->GetRotationYMatrix());
	}
	m_moveVec.Normalize();

	//速すぎるので補正して座標に代入
	if (!m_stepbeginFlg)
	{
		// 慣性を考慮して移動速度を計算
		if (m_groundFlg)
		{
			m_velocity = m_velocity * m_inertiaFactor + (GetMatrix().Backward() * m_speed) * (1.0f - m_inertiaFactor);
		}
		else
		{
			m_velocity = m_velocity * m_skyinertiaFactor + (GetMatrix().Backward() * m_speed) * (1.0f - m_skyinertiaFactor);
		}
	}
	else//ステップした瞬間
	{
		// 慣性を無視して、直接移動ベクトルを適用
		m_moveVec *= m_speed;
		if (m_moveVec.LengthSquared() == 0.0f)
		{
			m_moveVec = GetMatrix().Backward() * m_speed;
		}
		m_velocity = m_moveVec;
	}
	m_pos += m_velocity;

	// キャラクターの回転行列を創る
	//if (m_moveVec.LengthSquared() != 0.0f) { m_dir = m_moveVec; }
	m_dir = m_moveVec;
	m_dir.Normalize();
	UpdateRotate(m_dir);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = _rotation * Math::Matrix::CreateTranslation(m_pos);


	// キャラクターの座標が確定してからコリジョンによる位置補正を行う
	UpdateCollision();

	//ブーストエフェクト
	if (m_stepFlg&&!m_overheatFlg)
	{
		m_boostendefkFlg = false;
		if (m_boostefk)
		{
			bool _playFlg = m_boostefk->IsPlaying();
			if (!_playFlg)
			{
				m_boostefk = KdEffekseerManager::GetInstance().Play("boost/boost.efk", m_pos, m_worldRot, 1, 1, false).lock();
				AddEffect(m_boostefk);
			}
		}
	}
	else
	{
		if (!m_boostendefkFlg)
		{
			auto _spEffect = KdEffekseerManager::GetInstance().Play("boost/boostend.efk", m_pos, m_worldRot, 1, 1, false).lock();
			AddEffect(_spEffect);
		}
		m_boostendefkFlg = true;
	}

	//ジャンプエフェクト
	if(m_jumpefkFlg&&!m_overheatFlg)
	{
		m_jumpendefkFlg = false;
		if (m_jumpefk)
		{
			bool _playFlg = m_jumpefk->IsPlaying();
			if (!_playFlg)
			{
				m_jumpefk = KdEffekseerManager::GetInstance().Play("jump/jump.efk",m_pos, m_worldRot, 1, 1, false).lock();
				AddEffect(m_jumpefk);
			}
		}
	}
	else
	{
		if (!m_jumpendefkFlg)
		{
			auto _spEffect = KdEffekseerManager::GetInstance().Play("jump/jumpend.efk", m_pos, m_worldRot, 1, 1, false).lock();
			AddEffect(_spEffect);
		}
		m_jumpendefkFlg = true;
	}

}

//後更新
void Player::PostUpdate()
{

	if (m_boostgauge <= 0)
	{
		m_boostgauge = 0;
		m_overheatFlg = true;
	}
	//オーバーヒート処理
	if (m_overheatFlg)
	{
		if (m_boostgauge >= 200)
		{
			m_overheatFlg = false;
		}
	}

	//ブースト回復
	if (!m_boostFlg)
	{
		if (m_overheatFlg)
		{
			if (m_boostgauge < m_boostgaugeMax)
			{
				m_boostgauge += 1;
			}
		}
		else
		{
			if (m_boostgauge < m_boostgaugeMax)
			{
				if (m_groundFlg)
				{
					m_boostgauge += 5;
				}
				else
				{
					m_boostgauge += 1;
				}
			}
		}
	}

	//着地エフェクト
	if (m_groundFlg)
	{
		if (!m_LandingFlg)
		{
			KdEffekseerManager::GetInstance().Play("landing/landing.efk", m_pos, m_worldRot, 1, 1, false);
			m_LandingFlg = true;
		}
	}
	else
	{
		m_LandingFlg = false;
	}



	//アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();


	////「AttachPoint」ノードを取得する
	//if (m_spModel)
	//{
	//	//blenderで作成したNULLポイントノードを探して取得
	//	const KdModelWork::Node* _pNode = m_spModel->FindNode("APbattery");

	//	//m_APBatteryMat = Math::Matrix::CreateTranslation(_pNode->m_worldTransform.Translation());
	//	m_APBatteryMat = _pNode->m_worldTransform;
	//	//m_APBatteryMat = m_mWorld;

	//}

	//子オブジェクトにAPの座標渡し
	std::shared_ptr<Player_Battery> _spBattery = m_wpBattery.lock();
	if (_spBattery)
	{
		_spBattery->SetAPMatrix(m_APBatteryMat);

	}
}

//画像描画
void Player::DrawSprite()
{
	//ブーストゲージ関連
	m_barbackRc = { 0,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_barbackTex, 0, -300, 450, 100, &m_barbackRc);

	int boostbar;
	boostbar = m_boostgauge * 0.625f;//←　画像幅の長さ（５００）/　ブーストゲージで求まる
	m_barRc = { 500 - boostbar,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_boostTex, 9, -300, 450, 100, &m_barRc);

	m_barbackRc = { 0,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_barflontTex, 0, -300, 450, 100, &m_barbackRc);



	//HPゲージ関連
	m_barbackRc = { 0,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_barbackTex, -450, -320, 300, 80, &m_barbackRc);

	int hpbar;
	hpbar = m_hp * 25.0f;//←　画像幅の長さ（５００）/　ブーストゲージで求まる
	m_barRc = { 500 - hpbar,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_hpTex, -444, -320, 300, 80, &m_barRc);

	m_barbackRc = { 0,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_barflontTex, -450, -320, 300, 80, &m_barbackRc);

	{
		//円形描画テスト用（消す）
		KdTexture Tex;
		Math::Rectangle Rect;
		Math::Color Color;
		float angle = 360.0f - m_hp * (270.0f / m_hpMax);
		Tex.Load("Asset/Textures/test.png");
		Color = { 1,1,1,1 };
		Rect = { 0,0,128,128 };
		KdShaderManager::Instance().m_spriteShader.DrawTexAnime(&Tex, -500, -250, 128, 128, angle, &Rect, &Color);
	}


	//レティクル
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_reticleTex, 0, 0, 64, 64);
	

}

//imguiの更新
void Player::ImguiUpdate()
{
	ImGui::Text("player");
	ImGui::Text("boostgauge : %d", m_boostgauge);
	ImGui::Text("overhert : %d", m_overheatFlg);
	ImGui::Text("NowAction : %s", *m_nowAction);
	ImGui::Text("rotate : %f", m_worldRot.y);
	ImGui::Text("m_velocity.x : %f", m_velocity.x);
	ImGui::Text("m_velocity.y : %f", m_velocity.y);
	ImGui::Text("m_velocity.z : %f", m_velocity.z);
	ImGui::Text("m_velocityLength : %f", m_velocity.Length());
	ImGui::Text("m_groundflg : %d", m_groundFlg);
	ImGui::Text("m_jumpflg : %d", m_jumpFlg);
	ImGui::Text("m_floating : %f", m_floating);
	auto it = m_efkList.begin();
	while (it != m_efkList.end())
	{
		Application::Instance().m_log.AddLog("m_efkListh%d\n", (*it)->GetHandle());
		Application::Instance().m_log.AddLog("m_efkListh%f\n", (*it)->GetPos().x);
		++it;
	}
}

void Player::OnHit(const int _dmg)
{
	m_hp -= _dmg;
	if (m_hp <= 0)
	{
		m_destFlg = true;
		m_isExpired = true;
	}
}

void Player::CannonRecoil(const Math::Vector3 _dir,const float _str)
{
	Math::Vector3 _Dir = _dir;
	m_velocity = m_velocity + (_Dir * _str);
}

//カメラから見た移動方向に向く処理
void Player::UpdateRotate(const Math::Vector3& srcMoveVec)
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

	float rotateAng = 0;
	if (!m_stepbeginFlg)
	{
		rotateAng = std::clamp(_betweenAng, -8.0f, 8.0f);
	}
	else
	{
		rotateAng = std::clamp(_betweenAng, -180.0f, 180.0f);
	}
	m_worldRot.y += rotateAng;
	if (m_worldRot.y >= 360)
	{
		m_worldRot.y -= 360;
	}
	else if(m_worldRot.y < 0)
	{
		m_worldRot.y += 360;
	}
}

//当たり判定
void Player::UpdateCollision()
{
	// 地面判定

	static const float enableStepHigh = 2.0f;

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
	m_groundFlg = false;
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
				//ブースト回復
				m_groundFlg = true;

				//接地エフェクト
				static auto _spEffect = KdEffekseerManager::GetInstance().Play("floating/floating.efk", m_pos, m_worldRot, 0, 100, false).lock();
				if (_spEffect)
				{
					bool _playFlg = _spEffect->IsPlaying();
					if (!_playFlg)
					{
						_spEffect = KdEffekseerManager::GetInstance().Play("floating/floating.efk", m_pos, m_worldRot, 1, 2, false).lock();
					}
				}
			}
		}
	}

	// その他球による衝突判定
	// ---- ---- ---- ---- ---- ----
	// ①当たり判定(球判定)用の情報を作成
	KdCollider::SphereInfo sphereInfo;
	sphereInfo.m_sphere.Center = GetPos() + Math::Vector3(0,1.0f,0);
	sphereInfo.m_sphere.Radius = 2.0f;
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

void Player::UpdateJump()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !m_overheatFlg)
	{

		if (!m_jumpFlg)
		{
			m_jumpFlg = true;

			//ブースト消費
			if (m_boostgauge > 0)
			{
				m_boostgauge -= 50;
				m_boostFlg = true;
			}
		}

		if (m_jumpspeed <= m_jumpspeedMax)
		{
			m_jumpspeed += m_jumpspeedPow;
		}

		//ブースト消費
		if (m_boostgauge > 0)
		{
			m_boostgauge -= 2;
			m_boostFlg = true;
		}
		m_floating = m_jumpspeed;
	}
	else
	{
		m_jumpspeed = 0;
		m_jumpFlg = false;
		return;
	}
}

//---------------------------------------------------------------------------------------
//ここから下はステートパターン関係
//---------------------------------------------------------------------------------------
void Player::ChangeActionState(std::shared_ptr<ActionStateBase> nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextState;
	m_nowAction->Enter(*this);
}
//待機状態
void Player::ActionIdle::Enter(Player& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stop"), true);
}

void Player::ActionIdle::Update(Player& owner)
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !owner.m_overheatFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionJump>());
		return;
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && !owner.m_overheatFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionStep>());
		return;
	}

	if (GetAsyncKeyState('D') || GetAsyncKeyState('A') || GetAsyncKeyState('W') || GetAsyncKeyState('S'))
	{
		owner.ChangeActionState(std::make_shared<ActionMove>());
		return;
	}
}

void Player::ActionIdle::Exit(Player& owner)
{
}

void Player::ActionJump::Enter(Player& owner)
{
	owner.m_jumpspeed = owner.m_gravity;
	owner.m_jumpefkFlg = true;
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stop"), false);
}

void Player::ActionJump::Update(Player& owner)
{

	//ジャンプ
	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !owner.m_overheatFlg)
	{
		if (!owner.m_jumpFlg)
		{
			owner.m_jumpFlg = true;
			//ブースト消費
			if (owner.m_boostgauge > 0)
			{
				owner.m_boostgauge -= 50;
				owner.m_boostFlg = true;
			}
		}

		if (owner.m_jumpspeed <= owner.m_jumpspeedMax)
		{
			owner.m_jumpspeed += owner.m_jumpspeedPow;
		}

		//ブースト消費
		if (owner.m_boostgauge > 0)
		{
			owner.m_boostgauge -= 2;
			owner.m_boostFlg = true;
		}
		owner.m_floating = owner.m_jumpspeed;
	}
	else
	{
		owner.m_jumpspeed = 0;
		owner.m_jumpFlg = false;
		owner.ChangeActionState(std::make_shared<ActionIdle>());
		return;
	}


	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && !owner.m_overheatFlg && !owner.m_stepFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionStep>());
		return;
	}

	if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000))
	{
		owner.m_stepFlg = false;
	}

}

void Player::ActionJump::Exit(Player& owner)
{
	owner.m_jumpefkFlg = false;
}

void Player::ActionMove::Enter(Player& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stop"),false);

	owner.m_stepFlg = false;
	owner.m_jumpFlg = false;

}

void Player::ActionMove::Update(Player& owner)
{

	//入力が無ければ待機状態へ
	if (owner.m_moveVec.LengthSquared() == 0)
	{
		owner.ChangeActionState(std::make_shared<ActionIdle>());
		return;
	}

	owner.m_speed = owner.m_hoverspeed;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !owner.m_overheatFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionJump>());
		return;
	}

	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && !owner.m_overheatFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionStep>());
		return;
	}
}

void Player::ActionMove::Exit(Player& owner)
{
}

void Player::ActionStep::Enter(Player& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stop"), true);

	owner.m_stepbeginFlg = true;
	owner.m_stepFlg = false;
	owner.m_stepFlame = owner.m_stepFlamefixed;


	//SE再生
	KdAudioManager::Instance().Play("Asset/Sounds/Step.wav", false);
}

void Player::ActionStep::Update(Player& owner)
{
	owner.m_stepFlame--;
	//ステップ・ブースト
	if (owner.m_stepFlame > 0 || GetAsyncKeyState(VK_SHIFT) & 0x8000 && !owner.m_overheatFlg)
	{
		if (owner.m_moveVec.LengthSquared() == 0)
		{
			
		}

		if (!owner.m_stepFlg)
		{
			//ブースト消費
			if (owner.m_boostgauge > 0)
			{
				owner.m_boostgauge -= 50;
				owner.m_boostFlg = true;
			}
			owner.m_stepFlg = true;
			owner.m_speed = owner.m_stepspeed;
			//ステップエフェクト
			KdEffekseerManager::GetInstance().Play("step/step.efk", owner.m_pos, owner.m_worldRot, 1, 2, false);
		}
		else
		{
			owner.m_stepbeginFlg = false;

			//ブースト消費
			if (owner.m_boostgauge > 0)
			{
				owner.m_boostgauge -= 1;
				owner.m_boostFlg = true;
			}
			owner.m_speed = owner.m_boostspeed;
		}
	}

	//待機状態へ
	if (owner.m_stepFlame <= 0)
	{
		if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000) || owner.m_overheatFlg)
		{
			owner.m_stepFlg = false;
			owner.ChangeActionState(std::make_shared<ActionIdle>());
			return;
		}
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !owner.m_overheatFlg && !owner.m_jumpFlg)
	{
		if (owner.m_stepFlame <= 0)
		{
			owner.ChangeActionState(std::make_shared<ActionJump>());
			return;
		}
	}

	if (!(GetAsyncKeyState(VK_SPACE) & 0x8000))
	{
		owner.m_jumpFlg = false;
	}
}

void Player::ActionStep::Exit(Player& owner)
{
	owner.m_stepbeginFlg = false;
	owner.m_stepFlame = owner.m_stepFlamefixed;
}

void Player::ActionDrive::Enter(Player& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Idle"), true);
}

void Player::ActionDrive::Update(Player& owner)
{
	if (GetAsyncKeyState(VK_CONTROL))
	{

		owner.waitdrivetime--;
		if (owner.waitdrivetime <= 0)
		{
			owner.waitdrivetime = 0;
		}
		if (owner.waitdrivetime <= 0)
		{
			if (!owner.m_driveFlg)
			{
				owner.m_drivespeed = owner.m_drivespeedPow;

				owner.m_driveFlg = true;
			}
			else
			{
				owner.m_driveboostspeed = owner.m_driveboostspeedPow;
				if (owner.m_drivespeed > 0)
				{
					owner.m_drivespeed -= 1.0f;
				}
			}
		}
		owner.m_moveVec.z = 1.0f;

		owner.m_speed = owner.m_drivespeed + owner.m_driveboostspeed + 0.01f;

		//ブースト回復
		if (owner.m_boostgauge < owner.m_boostgaugeMax)
		{
			owner.m_boostgauge += 1;
		}
	}
}

void Player::ActionDrive::Exit(Player& owner)
{
}
