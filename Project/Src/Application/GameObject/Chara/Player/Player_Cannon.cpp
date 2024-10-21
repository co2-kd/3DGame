#include "Player_Cannon.h"
#include "../../../main.h"
#include"../../Camera/CameraBase.h"

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

	Math::Matrix _parentMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		_parentMat = m_localMat * _spParent->GetMatrix();
	}


	if (m_spModel)
	{
		//blenderで作成したNULLポイントノードを探して取得
		const KdModelWork::Node* _pNode = m_spModel->FindNode("APc_muzzle");

		//指定ノードが取得出来たら
		if (_pNode)
		{
			m_localMat = _pNode->m_worldTransform;
		}
	}
	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

//更新
void Player_Cannon::Update()
{
	//m_moveVec = Math::Vector3::Zero;

	////現在の状態の更新呼び出し
	//if (m_nowAction)
	//{
	//	m_nowAction->Update(*this);
	//}

	////カメラ取得
	//const std::shared_ptr<const CameraBase> _spCamera = m_wpCamera.lock();
	//if (_spCamera)
	//{
	//	m_moveVec = m_moveVec.TransformNormal(m_moveVec, _spCamera->GetRotationYMatrix());
	//}
	//m_moveVec.Normalize();

	//// キャラクターの回転行列を創る
	//m_dir = m_moveVec;
	//UpdateRotate(m_dir);

	//// キャラクターのワールド行列を創る処理
	//Math::Matrix _rotation = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_worldRot.y));
	//m_mWorld = Math::Matrix::CreateScale(0.2) * _rotation * Math::Matrix::CreateTranslation(m_pos);

	CharaBase::Update();

	Math::Matrix _Mat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		_Mat = m_localMat * _spParent->GetMatrix();
	}

	m_mWorld = _Mat;
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