#pragma once
#include"../Component.h"

class TransformComponent;

class SkydomeComponent:public Component
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;
	void PostUpdate()override;
	void DrawUnLit()override;
	const char* GetComponentName()const override { return "SkydomeComponent"; }

private:
	std::shared_ptr<KdModelData> m_spModel;
	std::shared_ptr<TransformComponent> m_transform;
	Math::Vector2 m_uvOffset1;
};