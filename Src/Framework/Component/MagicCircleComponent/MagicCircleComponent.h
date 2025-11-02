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
	void DrawLit()override;
	void DrawBright()override;

	void SetModel(const std::string& path);
	std::string GetModelPath() const { return m_modelPath; }
	void RequestTransformChangeCommand();
	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }

	const Math::Vector3& GetLocalPos()const { return m_localPos; }
	void SetLocalPos(const Math::Vector3& pos) { m_localPos = pos; }
	const Math::Vector3& GetLocalRot()const { return m_localRot; }
	void SetLocalRot(const Math::Vector3& rot) { m_localRot = rot; }
	const Math::Vector3& GetLocalScale()const { return m_localScale; }
	void SetLocalScale(const Math::Vector3& scale) { m_localScale = scale; }
	const float& GetOrbitRadius()const { return m_orbitRadius; }
	void SetOrbitRadius(const float& radius) { m_orbitRadius = radius; }
	const float& GetOrbitSpeed()const { return m_orbitSpeed; }
	void SetOrbitSpeed(const float& speed) { m_orbitSpeed = speed; }
	const Math::Vector3& GetOrbitAxisOffset()const { return m_orbitAxisOffset; }
	void SetOrbitAxisOffset(const Math::Vector3& offset) { m_orbitAxisOffset = offset; }

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
	float m_orbitAngle = 0.0f;
	float m_orbitSpeed = 45.0f;
	float m_orbitRadius = 1.5f;
	Math::Vector3 m_orbitAxisOffset = Math::Vector3::Zero;
};