#pragma once

class CharaBase : public KdGameObject
{
public:
	CharaBase() {}
	virtual ~CharaBase()override {}

	virtual void Update() override;

	virtual void DrawLit()override;
	virtual void GenerateDepthMapFromLight()override;

	void SetParent(const std::shared_ptr<KdGameObject>& parent)
	{
		m_wpParent = parent;
	}

	void SetTarget(const std::shared_ptr<KdGameObject>& target)
	{
		m_wpTarget = target;
	}

	void RegistHitObject(const std::shared_ptr<KdGameObject>& object)
	{
		m_wpHitObjectList.push_back(object);
	}

	virtual void SetAPMatrix(const Math::Matrix _localMat ={})
	{
		m_localMat = {};
	}

	//ゲッター
	const bool GetDestFlg()const override { return m_destFlg; }
	const Math::Vector3 GetWorldRot() const override { return m_worldRot; }
	const std::shared_ptr<KdModelWork> GetModel() const override { return m_spModel; }

protected:
	//モデル情報
	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	//親（持ち主）の情報
	std::weak_ptr<KdGameObject> m_wpParent;

	//標的の情報
	std::weak_ptr<KdGameObject> m_wpTarget;

	//当たり判定リスト(地形)
	std::vector<std::weak_ptr<KdGameObject>>	m_wpHitObjectList{};

	//親から本体へローカル行列（相対位置）
	Math::Matrix m_localMat;
	
	//座標
	Math::Vector3 m_pos;

	//移動方向ベクトル
	Math::Vector3 m_moveVec = Math::Vector3::Zero;

	//速度
	float m_speed = 0;

	//キャラが向きたい方向
	Math::Vector3 m_dir = Math::Vector3::Zero;

	//回転行列（）
	Math::Vector3 m_worldRot;

	//生存フラグ
	bool m_destFlg = false;

};