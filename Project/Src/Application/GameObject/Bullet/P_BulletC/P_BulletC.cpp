﻿#include "P_BulletC.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"

void P_BulletC::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::P_Bullet;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Bullet/P_BulletC/P_BulletC.gltf");
	}
	m_timelimit = 150.0f;
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void P_BulletC::Update()
{
	if (m_expFlg)
	{
		ExpCollision();
		m_isExpired = true;
		return;
	}

	//弾の生存時間
	m_timelimit--;
	if (m_timelimit < 0)
	{
		m_expFlg = true;
	}

	//移動回数
	//for (int i = 0; i < 3; i++)
	{
		//座標更新
		//座標 += 向き情報(1.0f) * 移動速度
		m_pos += m_dir * m_speed;

		//ワールド行列確定
		Math::Matrix _transMat;
		_transMat = Math::Matrix::CreateTranslation(m_pos);

		m_mWorld = m_rotBulletMat * _transMat;

		UpdateCollision();
	}
}

void P_BulletC::DrawBright()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void P_BulletC::UpdateCollision()
{

	{
		//球判定用の変数を作成
		KdCollider::SphereInfo sphereInfo;
		//球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos;
		//球の判定を設定
		sphereInfo.m_sphere.Radius = 5.0f;
		//当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeDamage;

		//デバッグ用
		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		}
		//全オブジェクトと当たり判定!!!!!
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->GetObjectType() == ObjectType::Player)continue;
			if (obj->Intersects(sphereInfo, nullptr))
			{
				m_expFlg = true;
			}
		}
	}

	{
		//球判定用の変数を作成
		KdCollider::SphereInfo sphereInfo;
		//球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos;
		//球の判定を設定
		sphereInfo.m_sphere.Radius = 1.0f;
		//当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeGround;

		//デバッグ用
		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		}
		//全オブジェクトと当たり判定!!!!!
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(sphereInfo, nullptr))
			{
				m_expFlg = true;
			}
		}
	}
}

void P_BulletC::ExpCollision()
{

	{
		//球判定用の変数を作成
		KdCollider::SphereInfo sphereInfo;
		//球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos;
		//球の判定を設定
		sphereInfo.m_sphere.Radius = 20.0f;
		//当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeDamage;

		//デバッグ用
		if (!(GetAsyncKeyState('Q') & 0x8000))
		{
			m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius, kRedColor);
		}
		//全オブジェクトと当たり判定!!!!!
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(sphereInfo, nullptr))
			{
				//敵なら当たった判定
				if (obj->GetObjectType() == ObjectType::Enemy)
				{
					obj->OnHit(m_dmg);
				}
			}
		}
	}
}