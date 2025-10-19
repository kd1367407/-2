#pragma once
#include"../Component.h"
#include"../ICollisionReceiver.h"
#include"../TransformComponent/TransformComponent.h"

class RotatingBlockComponent:public Component,public ICollisionReceiver
{
public:
	void Configure(const nlohmann::json& data) override;
	void Awake()override;
	void Start() override;
	void Update() override;
	void OnCollision(const CollisionInfo& info) override;
	void OnInspect() override;
	void RequestTransformChangeCommand();

	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }
	const char* GetComponentName() const override { return "RotatingBlockComponent"; }

	const Math::Vector3& GetRotatingAxis()const { return m_rotationAxis; }
	const float& GetRotatingAmount()const { return m_rotationAmount; }
	const float& GetRotatingSpeed()const { return m_rotationSpeed; }

	void SetRotatingAxis(const Math::Vector3& axis) { m_rotationAxis = axis; }
	void SetRotatingAmount(const float& amount) { m_rotationAmount = amount; }
	void SetRotatingSpeed(const float& speed) { m_rotationSpeed = speed; }

private:
	std::weak_ptr<TransformComponent> m_wpTransform;
	std::weak_ptr<GameViewModel> m_wpViewModel;

	//--パラメータ--
	Math::Vector3 m_rotationAxis = { 0.0f,1.0f,0.0f };//回転軸(デフォルト値はY軸)
	float m_rotationAmount = 90.0f;//一回の回転角度
	float m_rotationSpeed = 5.0f;//回転速度
	bool m_isRotating = false;
	Math::Quaternion m_targetRotation;//目標の回転
	float m_cooldownTimer = 0.0f;
};