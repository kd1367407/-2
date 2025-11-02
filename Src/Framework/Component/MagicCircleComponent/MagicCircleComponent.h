#pragma once
#include"../Component.h"

class TransformComponent;
class GameViewModel;

class MagicCircleComponent:public Component
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;
	void DrawBright()override;

	void SetModel(const std::string& path);
	void RequestTransformChangeCommand();

	void Configure(const nlohmann::json& data) override;
	nlohmann::json ToJson() const override;
	void OnInspect()override;
	const char* GetComponentName() const override { return "MagicCircleComponent"; }

private:
	std::shared_ptr<KdModelData> m_spModel;
	std::weak_ptr<GameViewModel> m_wpViewModel;
	std::string m_modelPath;
	std::shared_ptr<TransformComponent> m_ownerTransform;
	Math::Vector3 m_localPos = Math::Vector3::Zero;
	Math::Vector3 m_localRot = Math::Vector3::Zero;
	Math::Vector3 m_localScale = Math::Vector3::One;
	float m_rotationSpeedY = 90.0f;//90度/s
	bool m_isDirty = false;	
};