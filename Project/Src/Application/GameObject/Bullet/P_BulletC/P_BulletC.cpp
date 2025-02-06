#include "P_BulletC.h"
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
		//爆破エフェクト
		KdEffekseerManager::GetInstance().Play("cannonexp/cannonexp.efk", m_pos, {}, 3, 2, false);
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

	//軌跡エフェクト
	static auto _spEffect = KdEffekseerManager::GetInstance().Play("cannonray/cannonray.efk", m_pos, GetWorldRot(), 0, 1, false).lock();
	if (_spEffect)
	{
		bool _playFlg = _spEffect->IsPlaying();
		if (!_playFlg)
		{
			_spEffect = KdEffekseerManager::GetInstance().Play("cannonray/cannonray.efk", m_pos, GetWorldRot(), 1, 1, false).lock();
			AddEffect(_spEffect);
		}
	}
}


void P_BulletC::PostUpdate()
{

	// Updateの前の更新処理
	// オブジェクトリストの整理 ・・・ 無効なオブジェクトを削除
	auto efklist = KdEffekseerManager::GetInstance().GetnowEffectPlayList();
	auto efkit = efklist.begin();

	while (efkit != efklist.end())
	{
		if (!(*efkit)->IsPlaying())	// IsPlaying() ・・・ハンドルが0( 未再生 or 再生終了 )でない場合はTrue, 
		{
			auto it = m_efkList.begin();
			// 無効なエフェクトをリストから削除
			while (it != m_efkList.end())
			{
				if ((*it)->GetHandle() == (*efkit)->GetHandle())
				{
					//it = m_efkList.erase(it);
					break;
				}
				else
				{
					++it;	// 次の要素へイテレータを進める
				}
			}
			// 無効なエフェクトをリストから削除
			efkit = efklist.erase(efkit);

		}
		else
		{
			auto it = m_efkList.begin();
			while (it != m_efkList.end())
			{
				if ((*it)->GetHandle() == (*efkit)->GetHandle())
				{
					KdEffekseerManager::GetInstance().SetWorldMatrix((*it)->GetHandle(), m_mWorld);
					if (m_expFlg)
					{
						KdEffekseerManager::GetInstance().StopEffect((*it)->GetHandle());
					}
				}
				++it;
			}
			++efkit;	// 次の要素へイテレータを進める
		}
	}
	//m_efkList = KdEffekseerManager::GetInstance().GetnowEffectPlayList();

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
		//if (!(GetAsyncKeyState('Q') & 0x8000))
		//{
		//	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius);
		//}
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
		sphereInfo.m_sphere.Center = m_pos + Math::Vector3(m_dir * m_speed);
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
				obj->OnHit(m_dmg);
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
				obj->OnHit(m_dmg);
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
		//if (!(GetAsyncKeyState('Q') & 0x8000))
		//{
		//	m_pDebugWire->AddDebugSphere(sphereInfo.m_sphere.Center, sphereInfo.m_sphere.Radius, kRedColor);
		//}
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
