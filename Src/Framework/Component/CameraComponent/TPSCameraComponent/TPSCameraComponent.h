#pragma once
#include"../CameraComponent.h"

class TPSCameraComponent:public CameraComponent
{
public:
	void Awake() override;
	void Update() override;
	void PostUpdate() override;

	void SetTarget(const std::shared_ptr<TransformComponent>& target);

	const float& GetYaw()const { return m_yaw; }

	const char* GetComponentName() const override { return "TPSCameraComponent"; }

private:
	float m_sensitivity;
	std::weak_ptr<TransformComponent> m_wpTarget;
	Math::Matrix m_localMat;
	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	POINT m_fixMousePos = {};
};