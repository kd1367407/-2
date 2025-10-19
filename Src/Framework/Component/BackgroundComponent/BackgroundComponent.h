#pragma once
#include"../Component.h"

class BackgroundComponent:public Component
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;
	void DrawSprite()override;

	const char* GetComponentName()const override { return "BackgroundComponent"; }

private:
	Math::Vector2 m_uvOffset1;
	Math::Vector2 m_uvOffset2;
	std::shared_ptr<KdTexture> m_normalTex1;
	std::shared_ptr<KdTexture> m_normalTex2;
};