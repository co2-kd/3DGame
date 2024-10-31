#pragma once
#include"../CharaBase.h"

class CameraBase;
class Player_Minigun : public CharaBase
{
public:

	Player_Minigun()						{}
	~Player_Minigun()			override	{}

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
	void ShotBullet();

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 当たり判定と
	//void UpdateCollision();

	//std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;
	std::weak_ptr<CameraBase>			m_wpCamera;


	Math::Vector3 m_moveVec = Math::Vector3::Zero;


	//本体から銃口へのローカル行列
	Math::Matrix m_localmuzzleMat;

	bool m_shotFlg = false;

//ステートパターン関係
private:
	class ActionStateBase
	{
	public:
		virtual ~ActionStateBase() {}

		virtual void Enter(Player_Minigun& owner) {}
		virtual void Update(Player_Minigun& owner) {}
		virtual void Exit(Player_Minigun& owner) {}
	};




	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;

};