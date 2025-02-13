#include "TPSCamera.h"

void TPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 注視点
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 3.1f, -5.0f);

	SetCursorPos(m_FixMousePos.x, m_FixMousePos.y);
}

void TPSCamera::PostUpdate()
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat	= Math::Matrix::Identity;
	Math::Vector3								_targetVelo	= Math::Vector3::Zero;
	float										_targetVeloLength = 0.0f;
	float										_targetFloating = 0.0f;

	const std::shared_ptr<const KdGameObject>	_spTarget	= m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
		_targetVelo = _spTarget->GetVelocity();
		_targetVeloLength = _spTarget->GetVeloLength();
		_targetFloating = _spTarget->GetFloating();
	}

	// カメラの回転
	if ((GetAsyncKeyState(VK_TAB) & 0x8000))
	{
		if (!m_mouseFlg)
		{
			m_mouseFlg = true;
			ShowCursor(true);
		}
		else
		{
			m_mouseFlg = false;
			ShowCursor(false);
		}
	}


	if (!m_mouseFlg)
	{
		UpdateRotateByMouse();
	}
	//注視点
	m_LocalPosVelo = _targetVelo;
	m_LocalPosVelo.x = std::clamp(m_LocalPosVelo.x, m_OldLocalVelo.x - m_LocalVeloSpd, m_OldLocalVelo.x + m_LocalVeloSpd);
	m_LocalPosVelo.z = std::clamp(m_LocalPosVelo.z, m_OldLocalVelo.z - m_LocalVeloSpd, m_OldLocalVelo.z + m_LocalVeloSpd);
	m_LocalFloating = _targetFloating;
	m_LocalFloating = std::clamp(m_LocalFloating, m_OldFloating - m_LocalVeloSpd, m_OldFloating + m_LocalVeloSpd);
	m_LocalVeloLength = _targetVeloLength;
	m_LocalVeloLength = std::clamp(m_LocalVeloLength, m_OldLocalVeloLength - m_LocalVeloSpd, m_OldLocalVeloLength + m_LocalVeloSpd);


	m_mLocalPos = Math::Matrix::CreateTranslation(0.0f, 3.1f - m_LocalFloating, -(5.0f + (m_LocalVeloLength * 3)));
	m_LocalVeloMat = Math::Matrix::CreateTranslation(-m_LocalPosVelo.x * 3.0f, 0.0f, -m_LocalPosVelo.z * 3.0f);
	
	//カメラの移動量を保存
	m_OldLocalVelo = m_LocalPosVelo;
	m_OldFloating = m_LocalFloating;
	m_OldLocalVeloLength = m_LocalVeloLength;

	m_mRotation = GetRotationMatrix();
	m_mWorld	= m_mLocalPos * m_mRotation * m_LocalVeloMat * _targetMat;

	//当たり判定
	UpdateCollision();


	CameraBase::Update();
}

void TPSCamera::UpdateCollision()
{

	//// ↓めり込み防止の為の座標補正計算↓
	//// ①当たり判定(レイ判定)用の情報作成
	//KdCollider::RayInfo rayInfo;
	//// レイの発射位置を設定
	//rayInfo.m_pos = GetPos();

	//// レイの発射方向を設定
	//rayInfo.m_dir = Math::Vector3::Down;
	//// レイの長さを設定
	//rayInfo.m_range = 1000.f;
	//if (_spTarget)
	//{
	//	Math::Vector3 _targetPos = _spTarget->GetPos();
	//	_targetPos.y += 0.1f;
	//	rayInfo.m_dir = _targetPos - GetPos();
	//	rayInfo.m_range = rayInfo.m_dir.Length();
	//	rayInfo.m_dir.Normalize();
	//}

	//// 当たり判定をしたいタイプを設定
	//rayInfo.m_type = KdCollider::TypeGround;

	//// ②HIT判定対象オブジェクトに総当たり
	//for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	//{
	//	std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
	//	if (spGameObj)
	//	{
	//		std::list<KdCollider::CollisionResult> retRayList;
	//		spGameObj->Intersects(rayInfo, &retRayList);

	//		// ③ 結果を使って座標を補完する
	//		// レイに当たったリストから一番近いオブジェクトを検出
	//		float maxOverLap = 0;
	//		Math::Vector3 hitPos = {};
	//		bool hit = false;
	//		for (auto& ret : retRayList)
	//		{
	//			// レイを遮断しオーバーした長さが
	//			// 一番長いものを探す
	//			if (maxOverLap < ret.m_overlapDistance)
	//			{
	//				maxOverLap = ret.m_overlapDistance;
	//				hitPos = ret.m_hitPos;
	//				hit = true;
	//			}
	//		}
	//		if (hit)
	//		{
	//			// 何かしらの障害物に当たっている
	//			Math::Vector3 _hitPos = hitPos;
	//			_hitPos += rayInfo.m_dir * 0.4f;
	//			SetPos(_hitPos);
	//		}
	//	}
	//}

	// ↓めり込み防止の為の座標補正計算↓
	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();

	// レイの発射方向を設定
	rayInfo.m_dir = m_wpTarget.lock()->GetPos() - GetPos();
	rayInfo.m_range = rayInfo.m_dir.Length();//長さ
	rayInfo.m_dir.Normalize();

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	// ②HIT判定対象オブジェクトに総当たり
	for (std::weak_ptr<KdGameObject> wpGameObj : m_wpHitObjectList)
	{
		std::shared_ptr<KdGameObject> spGameObj = wpGameObj.lock();
		if (spGameObj)
		{
			std::list<KdCollider::CollisionResult> retRayList;
			spGameObj->Intersects(rayInfo, &retRayList);

			// ③ 結果を使って座標を補完する
			// レイに当たったリストから一番近いオブジェクトを検出
			float maxOverLap = 0;
			Math::Vector3 hitPos = {};
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
			if (hit)
			{
				// 何かしらの障害物に当たっている
				Math::Vector3 newCamPos = hitPos;
				newCamPos += rayInfo.m_dir * maxOverLap;
				SetPos(newCamPos);
			}
		}
	}

	// その他球による衝突判定
	// ---- ---- ---- ---- ---- ----
	// ①当たり判定(球判定)用の情報を作成
	KdCollider::SphereInfo sphereInfo;
	sphereInfo.m_sphere.Center = GetPos() + Math::Vector3(0.0f,0.0f,0.0f);
	sphereInfo.m_sphere.Radius = 3.0f;
	sphereInfo.m_type = KdCollider::TypeGround;

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
