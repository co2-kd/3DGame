#include "TurretBase.h"

void TurretBase::Update()
{
	m_pos = GetPos();
	const std::shared_ptr<const KdGameObject> _spParent = m_wpParent.lock();
	if (_spParent)
	{
		//m_TransAPMat = Math::Matrix::CreateTranslation(m_localMat.Translation()) * Math::Matrix::CreateTranslation(_spParent->GetMatrix().Translation());
		m_TransAPMat = m_localMat * _spParent->GetMatrix();
	}
	const std::shared_ptr<const KdGameObject> _spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		m_dir = m_pos - _spTarget->GetPos();
	}
}

void TurretBase::UpdateRotate(const Math::Vector3& srcMoveVec)
{

	// 何も入力が無い場合は処理しない
	if (srcMoveVec.LengthSquared() == 0.0f) { return; }

	// キャラの正面方向のベクトル
	Math::Vector3 _nowDir = GetMatrix().Forward();

	// 向きたい方向のベクトル
	Math::Vector3 _targetDir = srcMoveVec;

	_nowDir.Normalize();
	_targetDir.Normalize();

	float _nowAng = atan(_nowDir.y);
	_nowAng = DirectX::XMConvertToDegrees(_nowAng);

	float _targetAng = atan(_targetDir.y);
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
	m_worldRot.x += rotateAng;
	//砲塔の射角の制限
	m_worldRot.x = std::clamp(m_worldRot.x, -30.0f, 30.0f);
}
