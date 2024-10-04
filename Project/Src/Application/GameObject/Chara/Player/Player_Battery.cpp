#include "Player_Battery.h"
#include "../../../main.h"
#include"../../Camera/CameraBase.h"

#include"../Player/Player_Cannon.h"
#include"../Player/Player_Minigun.h"
#include"../Player/Player_Nuke-tube.h"

//初期化
void Player_Battery::Init()
{
	if (!m_spModel)
	{
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Player/Player_battery.gltf");


		//初期のアニメ―ション
		m_spAnimator = std::make_shared<KdAnimator>();

	}


	//「AttachPoint」ノードを取得する
	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNodeCannon = m_spModel->FindNode("APcannon");
		m_APCannonMat = _pNodeCannon->m_worldTransform;
		
		const KdModelWork::Node* _pNodeMinigun = m_spModel->FindNode("APminigun");
		m_APMinigunMat = _pNodeMinigun->m_worldTransform;
		
		const KdModelWork::Node* _pNodeNuketube = m_spModel->FindNode("APNuke-tube");
		m_APNuketubeMat = _pNodeNuketube->m_worldTransform;

	}


	//子オブジェクトにAPの座標渡し
	std::shared_ptr<Player_Cannon> _spCannon = m_wpCannon.lock();
	if (_spCannon)
	{
		_spCannon->SetAPMatrix(m_APCannonMat);
	}
	std::shared_ptr<Player_Minigun> _spMinigun = m_wpMinigun.lock();
	if (_spMinigun)
	{
		_spMinigun->SetAPMatrix(m_APMinigunMat);
	}
	std::shared_ptr<Player_Nuketube> _spNuketube = m_wpNuketube.lock();
	if (_spNuketube)
	{
		_spNuketube->SetAPMatrix(m_APNuketubeMat);
	}

	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

//更新
void Player_Battery::Update()
{

	//CharaBase::Update();
}

//後更新
void Player_Battery::PostUpdate()
{

	//アニメーションの更新
	m_spAnimator->AdvanceTime(m_spModel->WorkNodes());
	m_spModel->CalcNodeMatrices();

	Math::Matrix _Mat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		_Mat = Math::Matrix::CreateTranslation(m_localMat.Translation()) * Math::Matrix::CreateTranslation(_spParent->GetMatrix().Translation());
		//_Mat = m_localMat * Math::Matrix::CreateTranslation(_spParent->GetMatrix().Translation());
		//_Mat = m_localMat * _spParent->GetMatrix();
	}


	//カメラ取得
	m_dir.z = 1.0f;
	const std::shared_ptr<const CameraBase> _spCamera = m_wpCamera.lock();
	if (_spCamera)
	{
		m_dir = m_dir.TransformNormal(m_dir, _spCamera->GetRotationYMatrix());
	}
	m_dir.Normalize();
	// キャラクターの回転行列を創る
	UpdateRotate(m_dir);

	Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));

	m_mWorld = _rotation * _Mat;
	//m_mWorld = _Mat;



	//「AttachPoint」ノードを取得する
	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNodeCannon = m_spModel->FindNode("APcannon");
		m_APCannonMat = _pNodeCannon->m_worldTransform;

		const KdModelWork::Node* _pNodeMinigun = m_spModel->FindNode("APminigun");
		m_APMinigunMat = _pNodeMinigun->m_worldTransform;

		const KdModelWork::Node* _pNodeNuketube = m_spModel->FindNode("APNuke-tube");
		m_APNuketubeMat = _pNodeNuketube->m_worldTransform;

	}

	//子オブジェクトにAPの座標渡し
	std::shared_ptr<Player_Cannon> _spCannon = m_wpCannon.lock();
	if (_spCannon)
	{
		_spCannon->SetAPMatrix(m_APCannonMat);
	}
	std::shared_ptr<Player_Minigun> _spMinigun = m_wpMinigun.lock();
	if (_spMinigun)
	{
		_spMinigun->SetAPMatrix(m_APMinigunMat);
	}
	std::shared_ptr<Player_Nuketube> _spNuketube = m_wpNuketube.lock();
	if (_spNuketube)
	{
		_spNuketube->SetAPMatrix(m_APNuketubeMat);
	}
}

////モデル描画
//void Player_Battery::DrawLit()
//{
//	if (!m_spModel) return;
//
//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
//}
//
////影描画
//void Player_Battery::GenerateDepthMapFromLight()
//{
//	if (!m_spModel) return;
//
//	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
//}

//imguiの更新
void Player_Battery::ImguiUpdate()
{
	ImGui::Text("localx : %f", m_mWorld.Translation().x);
	ImGui::Text("localy : %f", m_mWorld.Translation().y);
	ImGui::Text("localz : %f", m_mWorld.Translation().z);

	ImGui::Text("rotate : %f", m_worldRot.y);
}

//カメラから見た移動方向に向く処理
void Player_Battery::UpdateRotate(const Math::Vector3& srcMoveVec)
{
	// 何も入力が無い場合は処理しない
	if (srcMoveVec.LengthSquared() == 0.0f) { return; }

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
	else if (m_worldRot.y < 0)
	{
		m_worldRot.y += 360;
	}
}

//当たり判定
void Player_Battery::UpdateCollision()
{

	// ①当たり判定(レイ判定)用の情報作成
	KdCollider::RayInfo rayInfo;
	// レイの発射位置を設定
	rayInfo.m_pos = GetPos();
	// 少し高い所から飛ばす(段差の許容範囲)
	rayInfo.m_pos.y += 0.4f;

	// レイの発射方向を設定
	rayInfo.m_dir = Math::Vector3::Down;

	// レイの長さ
	rayInfo.m_range = 0.2f;

	// 当たり判定をしたいタイプを設定
	rayInfo.m_type = KdCollider::TypeGround;

	m_pDebugWire->AddDebugLine(rayInfo.m_pos, rayInfo.m_dir, rayInfo.m_range);
}

//---------------------------------------------------------------------------------------
//ここから下はステートパターン関係
//---------------------------------------------------------------------------------------
void Player_Battery::ChangeActionState(std::shared_ptr<ActionStateBase> nextState)
{
	if (m_nowAction)m_nowAction->Exit(*this);
	m_nowAction = nextState;
	m_nowAction->Enter(*this);
}