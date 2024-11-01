﻿#include "Player.h"
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

	m_boostbackTex.Load("Asset/Textures/boost_bar_back.png");
	m_boostflontTex.Load("Asset/Textures/boost_bar_flont.png");
	m_boostTex.Load("Asset/Textures/boost_bar2.png");
	m_tex.Load("Asset/Textures/cross.png");

	//初期状態を「待機状態」へ設定
	ChangeActionState(std::make_shared<ActionIdle>());
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
		if (GetAsyncKeyState('D')) { m_moveVec.x = 1.0f; }
		if (GetAsyncKeyState('A')) { m_moveVec.x = -1.0f; }
		if (GetAsyncKeyState('W')) { m_moveVec.z = 1.0f; }
		if (GetAsyncKeyState('S')) { m_moveVec.z = -1.0f; }
		m_nowAction->Update(*this);
	}
	//カメラ取得
	const std::shared_ptr<const CameraBase> _spCamera = m_wpCamera.lock();
	if (_spCamera)
	{
		m_moveVec = m_moveVec.TransformNormal(m_moveVec, _spCamera->GetRotationYMatrix());
	}
	m_moveVec.Normalize();
	m_moveVec *= m_speed;
	m_pos += m_moveVec;
	//天井判定(仮)
	if (m_pos.y > 35.0f)
	{
		m_pos.y = 35.0f;
	}

	//m_moveVec.Normalize();
	//m_pos += GetMatrix().Backward() * m_speed;

	// キャラクターの回転行列を創る
	m_dir = m_moveVec;
	UpdateRotate(m_dir);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = _rotation * Math::Matrix::CreateTranslation(m_pos);


	// キャラクターの座標が確定してからコリジョンによる位置補正を行う
	UpdateCollision();
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
	m_boostbackRc = { 0,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_boostbackTex, 0, -300, 500, 100, &m_boostbackRc);

	int boostbar;
	boostbar = m_boostgauge * 0.625f;//←　画像幅の長さ（５００）・　ブーストゲージで求まる
	m_boostRc = { 500 - boostbar,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_boostTex, 10, -300, 500, 100, &m_boostRc);

	m_boostbackRc = { 0,0,500,100 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_boostflontTex, 0, -300, 500, 100, &m_boostbackRc);

	KdShaderManager::Instance().m_spriteShader.DrawTex(&m_tex, 0, 0, 64, 64);
}

//imguiの更新
void Player::ImguiUpdate()
{
	ImGui::Text("boostgauge : %d", m_boostgauge);
	ImGui::Text("overhert : %d", m_overheatFlg);
	ImGui::Text("NowAction : %s", *m_nowAction);
	ImGui::Text("rotate : %f", m_worldRot.y);
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

	float rotateAng = std::clamp(_betweenAng, -10.0f, 10.0f);
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
	rayInfo.m_type = KdCollider::TypeGround | KdCollider::TypeBump;
	if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);
	}

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
							m_boostgauge += 5;
						}
					}
				}
			}
		}
	}

	// その他球による衝突判定
	// ---- ---- ---- ---- ---- ----
	// ①当たり判定(球判定)用の情報を作成
	KdCollider::SphereInfo sphereInfo;
	sphereInfo.m_sphere.Center = GetPos() + Math::Vector3(0,1.5f,0);
	sphereInfo.m_sphere.Radius = 2.5f;
	sphereInfo.m_type = KdCollider::TypeGround | KdCollider::TypeBump;

	if (!(GetAsyncKeyState('Q') & 0x8000))
	{
		m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius, kRedColor);
	}

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
	//ジャンプ
	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !m_overheatFlg)
	{
		if (m_jumpFlg = false)
		{
			m_jumpspeed = m_gravity;
			m_jumpFlg = true;
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
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stop"), false);
}

void Player::ActionJump::Update(Player& owner)
{

	//ジャンプ
	if (GetAsyncKeyState(VK_SPACE) & 0x8000 && !owner.m_overheatFlg)
	{

		if (owner.m_jumpFlg = false)
		{
			owner.m_jumpFlg = true;
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


	if (GetAsyncKeyState(VK_SHIFT) & 0x8000 && !owner.m_overheatFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionStep>());
		return;
	}

	//if (GetAsyncKeyState('D') || GetAsyncKeyState('A') || GetAsyncKeyState('W') || GetAsyncKeyState('S'))
	//{
	//	owner.ChangeActionState(std::make_shared<ActionMove>());
	//	return;
	//}

	if (!(GetAsyncKeyState(VK_SPACE) & 0x8000) || owner.m_overheatFlg)
	{
		owner.ChangeActionState(std::make_shared<ActionIdle>());
		return;
	}
}

void Player::ActionJump::Exit(Player& owner)
{
}

void Player::ActionMove::Enter(Player& owner)
{
	owner.m_spAnimator->SetAnimation(owner.m_spModel->GetData()->GetAnimation("Stop"),false);
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

	//SE再生
	KdAudioManager::Instance().Play("Asset/Sounds/Step.wav", false);
}

void Player::ActionStep::Update(Player& owner)
{
	//ステップ・ブースト
	if (GetAsyncKeyState(VK_SHIFT) && !owner.m_overheatFlg)
	{

		if (!owner.m_stepFlg)
		{
			owner.m_stepspeed = owner.m_stepspeedPow;

			//ブースト消費
			if (owner.m_boostgauge > 0)
			{
				owner.m_boostgauge -= 50;
				owner.m_boostFlg = true;
			}
			owner.m_stepFlg = true;
		}
		else
		{
			owner.m_boostspeed = owner.m_boostspeedPow;
			if (owner.m_stepspeed > 0)
			{
				owner.m_stepspeed -= 0.5f;
			}

			//ブースト消費
			if (owner.m_boostgauge > 0)
			{
				owner.m_boostgauge -= 1;
				owner.m_boostFlg = true;
			}
		}
		owner.m_speed = owner.m_stepspeed + owner.m_boostspeed;
	}
	else
	{
		owner.m_stepFlg = false;
		if (owner.m_stepspeed > 0)
		{
			owner.m_stepspeed -= 0.5f;
		}
		if (owner.m_boostspeed > 0)
		{
			owner.m_boostspeed -= 0.05f;
		}
		owner.m_speed = owner.m_stepspeed + owner.m_boostspeed + owner.m_hoverspeed;
		owner.ChangeActionState(std::make_shared<ActionIdle>());
		return;
	}

}

void Player::ActionStep::Exit(Player& owner)
{
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
