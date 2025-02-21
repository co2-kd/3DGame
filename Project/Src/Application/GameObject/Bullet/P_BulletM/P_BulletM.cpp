﻿#include "P_BulletM.h"
#include"../../../Scene/SceneManager.h"
#include"../../../main.h"

void P_BulletM::Init()
{
	if (!m_spModel)
	{
		m_objectType = ObjectType::P_Bullet;
		m_spModel = std::make_shared<KdModelWork>();
		m_spModel->SetModelData("Asset/Models/Bullet/P_BulletM/P_BulletM.gltf");
	}

	m_pDebugWire = std::make_unique<KdDebugWireFrame>();
}

void P_BulletM::Update()
{
	//弾の生存時間
	m_timelimit--;
	if (m_timelimit <= 0)
	{
		//弾消滅
		m_isExpired = true;
		return;
	}

	//移動回数
	for (int i = 0; i < 2; i++)
	{
		//座標更新
		//座標 += 向き情報(1.0f) * 移動速度
		m_pos += m_dir * m_speed;

		//ワールド行列確定
		Math::Matrix _transMat;
		_transMat = Math::Matrix::CreateTranslation(m_pos);

		m_mWorld = m_rotBulletMat * _transMat;

		UpdateCollision();
		if (m_timelimit <= 0)
		{
			//弾消滅
			m_isExpired = true;
			return;
		}
	}
}

void P_BulletM::DrawBright()
{
	if (!m_spModel) return;

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
}

void P_BulletM::UpdateCollision()
{
	{
		//球判定用の変数を作成
		KdCollider::SphereInfo sphereInfo;
		//球の中心位置を設定
		sphereInfo.m_sphere.Center = m_pos + Math::Vector3(m_dir * m_speed);
		//球の判定を設定
		sphereInfo.m_sphere.Radius = 1.0f;
		//当たり判定をしたいタイプを設定
		sphereInfo.m_type = KdCollider::TypeGround | KdCollider::TypeDamage;

		////デバッグ用
		//if (!(GetAsyncKeyState('Q') & 0x8000))
		//{
		//	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		//}
		//全オブジェクトと当たり判定!!!!!
		for (auto& obj : SceneManager::Instance().GetObjList())
		{
			if (obj->Intersects(sphereInfo, nullptr))
			{
				if (obj->GetObjectType() == ObjectType::Player)continue;
				//敵なら当たった判定
				if (obj->GetObjectType() == ObjectType::Enemy)
				{
					obj->OnHit(m_dmg);
				}
				m_timelimit = 0;
				break;
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
		sphereInfo.m_type = KdCollider::TypeGround | KdCollider::TypeDamage;

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
				if (obj->GetObjectType() == ObjectType::Player)continue;
				//敵なら当たった判定
				if (obj->GetObjectType() == ObjectType::Enemy)
				{
					obj->OnHit(m_dmg);
				}
				m_timelimit = 0;
				break;
			}
		}
	}
}
