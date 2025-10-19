#pragma once
#include"../../Component.h"
#include"../ICameraComponent/ICameraComponent.h"

class TransformComponent;

class EditorCameraComponent:public Component,public ICameraComponent, public std::enable_shared_from_this<EditorCameraComponent>
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;

	const std::shared_ptr<KdCamera>& GetCamera() const override { return m_spCamera; }
	GameObject* GetOwner() const override { return m_owner; }

	float GetYaw() const { return m_yaw; }
	float GetPitch() const { return m_pitch; }

	const char* GetComponentName()const override { return "EditorCameraComponent"; }

private:
	std::shared_ptr<KdCamera> m_spCamera;
	std::shared_ptr<TransformComponent> m_transform;

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	POINT m_preMousePos = {};

	float m_sensitivity;
	float m_moveSpeed;
};