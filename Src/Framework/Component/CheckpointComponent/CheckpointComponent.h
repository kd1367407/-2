#pragma once
#include"../Component.h"
#include"../ICollisionReceiver.h"

class CheckpointComponent:public Component,public ICollisionReceiver
{
public:
	void Awake()override;
	void Start() override;
	void OnCollision(const CollisionInfo& info) override;

	const char* GetComponentName() const override { return "CheckpointComponent"; }

private:

};