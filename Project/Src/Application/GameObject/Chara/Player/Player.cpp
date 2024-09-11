#include "Player.h"

#include "../../../main.h"
#include"../../Camera/CameraBase.h"


void Player::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Player/Player3.gltf");
		m_pos.y = 10;
		//デバッグ用
		m_pDebugWire = std::make_unique<KdDebugWireFrame>();

		//初期のアニメ―ション
		m_spAnimator = std::make_shared<KdAnimator>();
		m_spAnimator->SetAnimation(m_spModel->GetData()->GetAnimation("Idle"),true);

	}

	m_boostbackTex.Load("Asset/Textures/boost_bar_back.png");
	m_boostflontTex.Load("Asset/Textures/boost_bar_flont.png");
	m_boostTex.Load("Asset/Textures/boost_bar2.png");
}

void Player::Update()
{

	m_boostFlg = false;
	
	//重力
	m_pos.y -= m_gravity;
	//重力加算
	if (m_gravity <= m_gravityMax)
	{
		m_gravity += m_gravityPow;
	}
	m_moveVec = Math::Vector3::Zero;

	//ドライブブースト
	if (GetAsyncKeyState(VK_CONTROL))
	{

		waitdrivetime--;
		if (waitdrivetime<=0)
		{
			waitdrivetime = 0;
		}
		if (waitdrivetime <= 0)
		{
			if (!m_driveFlg)
			{
				m_drivespeed = m_drivespeedPow;

				m_driveFlg = true;
			}
			else
			{
				m_driveboostspeed = m_driveboostspeedPow;
				if (m_drivespeed > 0)
				{
					m_drivespeed -= 1.0f;
				}
			}
		}
		m_moveVec.z = 1.0f;

		m_speed = m_drivespeed + m_driveboostspeed + 0.01f;

		//ブースト回復
		if (m_boostgauge < m_boostgaugeMax)
		{
			m_boostgauge += 1;
		}
	}
	//非ドライブブースト
	else
	{
		waitdrivetime = 60;
		m_driveFlg = false;
		if (m_drivespeed > 0)
		{
			m_drivespeed -= 1.0f;
			m_speed = m_drivespeed + m_driveboostspeed;
		}
		m_driveboostspeed = 0;



		if (GetAsyncKeyState('D')) { m_moveVec.x = 1.0f; }
		if (GetAsyncKeyState('A')) { m_moveVec.x = -1.0f; }
		if (GetAsyncKeyState('W')) { m_moveVec.z = 1.0f; }
		if (GetAsyncKeyState('S')) { m_moveVec.z = -1.0f; }
		m_speed = m_hoverspeed;

		//ステップ・ブースト
		if (GetAsyncKeyState(VK_SHIFT) && !m_overheatFlg)
		{
			if (!m_stepFlg)
			{
				m_stepspeed = m_stepspeedPow;

				//ブースト消費
				if (m_boostgauge > 0)
				{
					m_boostgauge -= 50;
					m_boostFlg = true;
				}
				m_stepFlg = true;
			}
			else
			{
				m_boostspeed = m_boostspeedPow;
				if (m_stepspeed > 0)
				{
					m_stepspeed -= 0.5f;
				}

				//ブースト消費
				if (m_boostgauge > 0)
				{
					m_boostgauge -= 1;
					m_boostFlg = true;
				}
			}
			m_speed = m_stepspeed + m_boostspeed;
		}
		else
		{
			m_stepFlg = false;
			if (m_stepspeed > 0)
			{
				m_stepspeed -= 0.5f;
			}
			if (m_boostspeed > 0)
			{
				m_boostspeed -= 0.05f;
			}
			m_speed = m_stepspeed + m_boostspeed + m_hoverspeed;
		}

		//ジャンプ
		if (GetAsyncKeyState(VK_SPACE) && !m_overheatFlg)
		{
			if (m_jumpspeed <= m_jumpspeedMax)
			{
				m_jumpspeed += m_jumpspeedPow;
			}

			//ブースト消費
			if (m_boostgauge > 0)
			{
				m_boostgauge -= 1;
				m_boostFlg = true;
			}
			m_gravity = -m_jumpspeed;
		}
		else
		{
			if (m_jumpspeed >= 0)
			{
				m_jumpspeed -= m_jumpspeedPow / 5;
			}

		}
	}

	const std::shared_ptr<const CameraBase> _spCamera = m_wpCamera.lock();
	if (_spCamera)
	{
		m_moveVec = m_moveVec.TransformNormal(m_moveVec, _spCamera->GetRotationYMatrix());
	}
	m_moveVec.Normalize();
	m_moveVec *= m_speed;
	m_pos += m_moveVec;

	// キャラクターの回転行列を創る
	m_dir = m_moveVec;
	UpdateRotate(m_dir);

	// キャラクターのワールド行列を創る処理
	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	m_mWorld = Math::Matrix::CreateScale(0.3) * _rotation * Math::Matrix::CreateTranslation(m_pos);

	// キャラクターの座標が確定してからコリジョンによる位置補正を行う
	UpdateCollision();


}

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
				m_boostgauge += 3;
			}
		}
	}

	//アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();
}

void Player::DrawLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void Player::GenerateDepthMapFromLight()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

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

}

void Player::ImguiUpdate()
{
	ImGui::Text("boostgauge : %d", m_boostgauge);
	ImGui::Text("overhert : %d", m_overheatFlg);
}

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
}

void Player::UpdateCollision()
{
	// 地面判定

	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高い所から飛ばす(段差の許容範囲)
	rayInfo.m_pos.y += 0.2f;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さ
	rayInfo.m_range = m_gravity + 0.2f;

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);

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
				SetPos(hitPos);
				m_gravity = 0;
			}
		}
	}

	// その他球による衝突判定
	// ---- ---- ---- ---- ---- ----
	// ①当たり判定(球判定)用の情報を作成
	KdCollider::SphereInfo sphereInfo;
	sphereInfo.m_sphere.Center = GetPos() + Math::Vector3(0, 0.5f, 0);
	sphereInfo.m_sphere.Radius = 0.5f;
	sphereInfo.m_type = KdCollider::TypeBump;

	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius, kRedColor);

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