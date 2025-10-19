#pragma once
#include"../Component.h"

class CommandInvoker;
class RigidbodyComponent;
class GravityComponent;
class TPSCameraComponent;

class PlayerInputComponent:public Component
{
public:
	PlayerInputComponent() { m_updatePriority = 100; }//rigidとgravityが必要なのでそれらより後に実行

	void Awake()override;
	void Start()override;
	
	void Update()override;

	const char* GetComponentName()const override { return "PlayerInputComponent"; }

	//操作命令を送る先のInvokerを設定
	void SetInvoker(std::shared_ptr<CommandInvoker> invoker);

	void SetCamera(std::shared_ptr<TPSCameraComponent> spCamera) { m_wpCamera = spCamera; }

private:
	std::shared_ptr<CommandInvoker> m_invoker;
	std::shared_ptr<RigidbodyComponent> m_rigid;
	std::shared_ptr<GravityComponent> m_gravity;
	std::weak_ptr<TPSCameraComponent> m_wpCamera;
	float m_coyoteTimeCounter = 0.0f;
	float m_maxSpeed;
	float m_moveForce;
	float m_controlForce;
	float m_brakeForce;
	float m_jumpPower;
	float m_coyoteTimeDuration;
};