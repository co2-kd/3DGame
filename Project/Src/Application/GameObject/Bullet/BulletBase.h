#pragma once

class BulletBase : public KdGameObject
{
public:
	BulletBase() {}
	virtual ~BulletBase()override {}

	virtual void DrawUnLit()override;

	//void SetParent(const std::shared_ptr<KdGameObject>& parent)
	//{
	//	m_wpParent = parent;
	//}

	virtual void Shot(const Math::Vector3 _pos, const Math::Vector3 _dir);


protected:
	//モデル情報
	std::shared_ptr<KdModelWork>m_spModel = nullptr;

	//ワールド座標
	Math::Vector3 m_pos;

	//ベクトル方向
	Math::Vector3 m_dir;


	//照準への回転行列
	Math::Matrix m_rotBulletMat;
};