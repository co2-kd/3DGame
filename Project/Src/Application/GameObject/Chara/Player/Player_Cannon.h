#pragma once
#include"../CharaBase.h"

class CameraBase;
class Player_Cannon : public CharaBase
{
public:

	Player_Cannon()						{}
	~Player_Cannon()			override	{}

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
	Math::Vector3 m_muzzlePos;


	bool m_shotFlg = false;


//ステートパターン関係
private:
	class ActionStateBase
	{
	public:
		virtual ~ActionStateBase() {}

		virtual void Enter(Player_Cannon& owner) {}
		virtual void Update(Player_Cannon& owner) {}
		virtual void Exit(Player_Cannon& owner) {}
	};

	//射撃状態
	class ActionShoting : public ActionStateBase
	{
	public:
		~ActionShoting() {}

		void Enter(Player_Cannon& owner) override;
		void Update(Player_Cannon& owner) override;
		void Exit(Player_Cannon& owner) override;
	};
	//射撃終了状態
	class ActionUnShot : public ActionStateBase
	{
	public:
		~ActionUnShot() {}

		void Enter(Player_Cannon& owner) override;
		void Update(Player_Cannon& owner) override;
		void Exit(Player_Cannon& owner) override;
	};


	void ChangeActionState(std::shared_ptr<ActionStateBase> nextState);
	std::shared_ptr<ActionStateBase> m_nowAction = nullptr;

};