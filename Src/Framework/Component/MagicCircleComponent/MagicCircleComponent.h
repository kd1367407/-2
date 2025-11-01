#pragma once
#include"../Component.h"

class TransformComponent;

class MagicCircleComponent:public Component
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;
	void DrawLit()override;

	void Configure(const nlohmann::json& data) override;
	nlohmann::json ToJson() const override;
	const char* GetComponentName() const override { return "MagicCircleComponent"; }

private:
	std::shared_ptr<KdModelData> m_spModel;
	std::string modelPath;
	std::shared_ptr<TransformComponent> m_ownerTransform;
	Math::Vector3 m_localPos = Math::Vector3::Zero;
	Math::Vector3 m_localRot = Math::Vector3::Zero;
	Math::Vector3 m_localScale = Math::Vector3::One;
	float m_rotationSpeedY = 90.0f;//90度/s
};