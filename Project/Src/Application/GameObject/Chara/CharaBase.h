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

	virtual void SetAPMatrix(const Math::Matrix _localMat ={})
	{
		m_localMat = {};
	}

protected:
	//モデル情報
	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	//親（持ち主）の情報
	std::weak_ptr<KdGameObject> m_wpParent;

	//親から本体へローカル行列（相対位置）
	Math::Matrix m_localMat;
	
	//座標
	Math::Vector3 m_pos;
};