#pragma once
class Wall : public KdGameObject
{
public:
	Wall() {}
	 virtual~Wall() override{}

	void Init()override;
	void Update()override;
	void DrawLit()override;
	void OnHit(const int _dmg)override;

	void ImguiUpdate()override;

private:
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

	bool m_destFlg = false;

	//HP
	const int m_hpMax = 100;
	int m_hp = m_hpMax;
};