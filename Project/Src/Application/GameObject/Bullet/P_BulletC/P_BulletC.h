#pragma once
#include"../BulletBase.h"

class P_BulletC :public BulletBase
{
public:
	P_BulletC(){}
	~P_BulletC()override {}

	void Init() override;

	void Update()override;

	void PostUpdate()override;

	void DrawBright() override;

	//エフェクト関係
	// オブジェクトリストを取得
	const std::list<std::shared_ptr<KdEffekseerObject>>& GetEfkList()
	{
		return m_efkList;
	}

	// オブジェクトリストに追加
	void AddEffect(const std::shared_ptr<KdEffekseerObject>& efk)
	{
		m_efkList.push_back(efk);
	}

private:
	// 当たり判定
	void UpdateCollision();

	//着弾時の爆発
	void ExpCollision();

	bool m_expFlg = false;

	//速度
	const float m_speed = 2.0f;

	//生存時間
	int m_timelimit;

	//ダメージ
	const int m_dmg = 10;

	//エフェクト関係
	std::list<std::shared_ptr<KdEffekseerObject>> m_efkList;
};