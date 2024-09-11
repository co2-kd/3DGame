#pragma once

class CameraBase;
class Player : public KdGameObject
{
public:
	enum DirType
	{
		Up = 1 << 0,		//上
		Down = 1 << 1,	//下
		Left = 1 << 2,	//左
		Right = 1 << 3,	//右
		None = 1 << 4
	};

	Player()						{}
	~Player()			override	{}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;

	void DrawLit()			override;
	void GenerateDepthMapFromLight() override;
	void DrawSprite()       override;

	void ImguiUpdate()override;

	void SetCamera(const std::shared_ptr<CameraBase>& camera)
	{
		m_wpCamera = camera;
	}

	void RegistHitObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_wpHitObjectList.push_back(object);
	}

private:

	void UpdateRotate(const Math::Vector3& srcMoveVec);

	// 衝突判定とそれに伴う座標の更新
	void UpdateCollision();

	KdTexture m_boostTex;
	KdTexture m_boostbackTex;
	KdTexture m_boostflontTex;

	Math::Rectangle m_boostRc;
	Math::Rectangle m_boostbackRc;

	Math::Matrix m_boostMat;

	std::shared_ptr<KdModelWork>	m_spModel	= nullptr;
	std::shared_ptr<KdAnimator> m_spAnimator = nullptr;
	std::weak_ptr<CameraBase>			m_wpCamera;
	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};
	Math::Vector3						m_worldRot;

	//キャラが向いている方向
	Math::Vector3 m_dir = Math::Vector3::Zero;

	float m_speed = 0;
	Math::Vector3 m_moveVec = Math::Vector3::Zero;
	Math::Vector3 m_pos = Math::Vector3::Zero;

	float m_gravity = 0;
	const float m_gravityPow = 0.2f;
	const float m_gravityMax = 10.0f;

	//ホバー
	const float m_hoverspeed = 0.6f;
	
	//ジャンプ
	float m_jumpspeed = 0;
	const float m_jumpspeedPow = 3.0f;
	const float m_jumpspeedMax = 3.0f;

	//ステップ・ブースト
	bool m_stepFlg = false;

	float m_stepspeed = 0;
	const float m_stepspeedPow = 15.0f;

	float m_boostspeed = 0;
	const float m_boostspeedPow = 3.0f;

	//ドライブブースト
	bool m_driveFlg = false;

	int waitdrivetime = 0;

	
	float m_drivespeed = 0;
	const float m_drivespeedPow = 30.0f;

	float m_driveboostspeed = 0;
	const float m_driveboostspeedPow = 10.0f;

	//ブーストゲージ
	int m_boostgauge = 800;
	const int m_boostgaugeMax = 800;

	bool m_overheatFlg = false;

	bool m_boostFlg = false;
};