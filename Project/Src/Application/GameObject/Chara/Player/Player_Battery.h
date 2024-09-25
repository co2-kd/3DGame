#pragma once
#include"../CharaBase.h"

class CameraBase;
class Player_Battery : public CharaBase
{
public:

	Player_Battery()						{}
	~Player_Battery()			override	{}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	//void DrawLit()			override;
	//void GenerateDepthMapFromLight() override;

	void ImguiUpdate()override;

	void SetCamera(const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

	void SetAPMatrix(const Math::Matrix _localMat)override
	{
		m_localMat = _localMat;
	}

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 当たり判定と
	void UpdateCollision();

	//std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;
	std::weak_ptr<CameraBase>			m_wpCamera;
	Math::Vector3						m_worldRot;


	Math::Vector3 m_moveVec = Math::Vector3::Zero;

	//キャラが向いている方向
	Math::Vector3 m_dir = Math::Vector3::Zero;


	Math::Vector3 m_pos = Math::Vector3::Zero;


//ステートパターン関係
private:
	class ActionStateBase
	{
	public:
		virtual ~ActionStateBase() {}

		virtual void Enter(Player_Battery& owner) {}
		virtual void Update(Player_Battery& owner) {}
		virtual void Exit(Player_Battery& owner) {}
	};


	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;

};