#pragma once
#include "../CameraBase.h"

class TPSCamera : public CameraBase
{
public:
	TPSCamera()							{}
	~TPSCamera()			override	{}

	void Init()				override;
	void PostUpdate()			override;

private:

	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();

	Math::Matrix m_LocalVeloMat = Math::Matrix::Identity;
	Math::Vector3 m_OldLocalVelo = Math::Vector3::Zero;
	Math::Vector3 m_LocalPosVelo = Math::Vector3::Zero;
	const float m_LocalVeloSpd = 0.1f;

	float m_LocalVeloLength = 0.0f;
	float m_OldLocalVeloLength = 0.0f;

	float m_LocalFloating = 0.0f;
	float m_OldFloating = 0.0f;
};