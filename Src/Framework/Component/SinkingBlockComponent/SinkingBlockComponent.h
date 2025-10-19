#pragma once
#include"../Component.h"
#include"../ICollisionReceiver.h"

class TransformComponent;
class GameViewModel;

class SinkingBlockComponent:public Component,public ICollisionReceiver
{
public:
	void Configure(const nlohmann::json& data) override;
	void Awake()override;
	void Start() override;
	void Update() override;
	void OnCollision(const CollisionInfo& info) override;
	void OnInspect() override;
	nlohmann::json ToJson() const override;
	bool OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene)override;
	void RequestTransformChangeCommand();

	const char* GetComponentName() const override { return "SinkingBlockComponent"; }

	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }

	const Math::Vector3& GetInitialPos()const { return m_initialPos; }
	const float& GetMaxSinkDistance()const { return m_maxSinkDistance; }
	const float& GetAcceleration()const { return m_acceleration; }
	const float& GetRiseSpeed()const { return m_riseSpeed; }
	void SetInitialPos(const Math::Vector3& pos) { m_initialPos = pos; }
	void SetMaxSinkDistance(const float& distance) { m_maxSinkDistance = distance; }
	void SetAcceleration(const float& value) { m_acceleration = value; }
	void SetRiseSpeed(const float& speed) { m_riseSpeed = speed; }

private:
	std::weak_ptr<TransformComponent> m_wpTransform;
	std::weak_ptr<GameViewModel> m_wpViewModel;

	//--パラメータ--
	float m_maxSinkDistance = 2.0f;
	float m_acceleration = 3.0f;
	float m_riseSpeed = 1.0f;
	Math::Vector3 m_initialPos = {};

	//--状態変数--
	float m_currentSpeed = 0.0f;
	bool m_isPlayerOnTop = false;
	float m_playerOffTimer = 0.0f;

	//--ギズモ--
	bool m_isInitialGizmoDragging = false;
};