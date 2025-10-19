#pragma once
#include"../Component.h"

class MousePointerComponent:public Component
{
public:
	void Awake()override;
	void Start()override;
	void DrawSprite()override;

	void Configure(const nlohmann::json& data);
	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "MousePointerComponent"; }

private:
	std::shared_ptr<KdTexture> m_spTex;
	std::string m_texturePath;
};