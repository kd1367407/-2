#pragma once
#include"../Component.h"

class RigidbodyComponent;

class GravityComponent :public Component
{
public:
	GravityComponent() { m_updatePriority = 50; }//Rigidbodyを使うためrigidbodyより後に実行

	void Awake()override;
	void Start()override;
	void Update()override;
	void PostUpdate()override;

	void CheckGround();

	bool IsOnGround() { return m_isOnGround; }

	float GetGravity()const { return m_gameGravity; }

	const char* GetComponentName()const override { return "GravityComponent"; }

private:
	std::shared_ptr<RigidbodyComponent> m_rigidbody;
	bool m_isOnGround = false;
	float m_fallTimer = 0.0f;
	float m_fallTimeLimit = 5.0f;
	float m_gameGravity = 20.0f;
};
