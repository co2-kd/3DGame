#pragma once
class Stage1 : public KdGameObject
{
public:
	Stage1() {}
	 virtual~Stage1() override{}

	void Init()override;
	void DrawLit()override;

	void ImguiUpdate()override;

private:
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

};