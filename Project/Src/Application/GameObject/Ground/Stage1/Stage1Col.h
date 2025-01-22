#pragma once
class Stage1Col : public KdGameObject
{
public:
	Stage1Col() {}
	 virtual~Stage1Col() override{}

	void Init()override;
	void DrawLit()override;

	void ImguiUpdate()override;

private:
	std::shared_ptr<KdModelWork> m_spModel = nullptr;

};