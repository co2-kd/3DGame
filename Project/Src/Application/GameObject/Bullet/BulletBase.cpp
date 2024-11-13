#include "BulletBase.h"

void BulletBase::DrawUnLit()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void BulletBase::Shot(const Math::Vector3 _pos, const Math::Vector3 _dir)
{
	m_pos = _pos;
	m_dir = _dir;
	m_dir.Normalize();

	//照準へのベクトルと弾の進行ベクトルから弾の向き（回転行列）を作成

	//①ベクトルA…弾の現在進行ベクトル
	Math::Vector3 _vecA = m_mWorld.Backward();
	_vecA.Normalize();
	//②ベクトルB…照準への進行ベクトル
	Math::Vector3 _vecB = m_dir;
	_vecB.Normalize();

	//①と②のベクトルの内積値から角度を算出
	float _dot = _vecA.Dot(_vecB);	//内積値を算出
	float _angle = acos(_dot);	// 内積値から２つのベクトルの角度を算出

	//①と②のベクトルの外積を利用して、回転するためのベクトル（回転軸）を作成
	Math::Vector3 _rotAxis = _vecA.Cross(_vecB);

	//内積で算出した角度分、外積で算出したベクトル（回転軸）で回転する行列を作成
	//＝　弾がレティクルの方向を向く
	m_rotBulletMat = Math::Matrix::CreateFromAxisAngle(_rotAxis, _angle);
}

void BulletBase::OnHit(const int _dmg)
{
	m_isExpired = true;
}
