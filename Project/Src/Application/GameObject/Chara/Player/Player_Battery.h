#pragma once
#include"../CharaBase.h"

class CameraBase;
class Player_Cannon;
class Player_Minigun;
class Player_Nuketube;
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

	void SetCannon(const std::shared_ptr<Player_Cannon>& cannon)
	{
		m_wpCannon = cannon;
	}

	void SetMinigun(const std::shared_ptr<Player_Minigun>& minigun)
	{
		m_wpMinigun = minigun;
	}

	void SetNuketube(const std::shared_ptr<Player_Nuketube>& nuketube)
	{
		m_wpNuketube = nuketube;
	}

	//ゲッター
	const Math::Matrix GetAPCannonMatrix()const { return m_APCannonMat; }//アタッチポイント
	const Math::Matrix GetAPMinigunMatrix()const { return m_APMinigunMat; }//アタッチポイント
	const Math::Matrix GetAPNuketubeMatrix()const { return m_APNuketubeMat; }//アタッチポイント

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 当たり判定と
	//void UpdateCollision();

	//std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;
	std::weak_ptr<CameraBase>			m_wpCamera;


	Math::Vector3 m_moveVec = Math::Vector3::Zero;


	//アタッチポイント関係
	std::weak_ptr<Player_Cannon> m_wpCannon;
	Math::Matrix m_APCannonMat = {};
	std::weak_ptr<Player_Minigun> m_wpMinigun;
	Math::Matrix m_APMinigunMat = {};
	std::weak_ptr<Player_Nuketube> m_wpNuketube;
	Math::Matrix m_APNuketubeMat = {};

	const int m_Miniguncool = 5;
	int m_Miniguncount = 0;
	const int m_Cannoncool = 180;
	int m_Cannoncount = 0;


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