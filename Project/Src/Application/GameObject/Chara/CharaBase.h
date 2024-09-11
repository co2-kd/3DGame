#pragma once

class CharaBase : public KdGameObject
{
public:
	CharaBase() {}
	virtual ~CharaBase()override {}

	void DrawLit()override;
	void GenerateDepthMapFromLight()override;

protected:
	//モデル情報
	std::shared_ptr<KdModelWork>m_spModel = nullptr;
	
	//座標
	Math::Vector3 m_pos;
};