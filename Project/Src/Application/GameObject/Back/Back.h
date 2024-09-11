#pragma once

class Back : public KdGameObject
{
public:
	Back() {}
	~Back()override {}

	void Init()				override;
	void Update()			override;
	void PostUpdate()		override;
	void DrawUnLit()			override;

private:

	std::shared_ptr<KdModelWork> m_spModel = nullptr;
};