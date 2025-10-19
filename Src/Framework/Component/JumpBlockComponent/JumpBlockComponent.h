#pragma once
#include"../Component.h"
#include"../ICollisionReceiver.h"

class TransformComponent;
class RigidbodyComponent;
class GameViewModel;

class JumpBlockComponent:public Component,public ICollisionReceiver
{
public:
	JumpBlockComponent() { m_updatePriority = -101; }
	void Awake()override;
	void Start()override;
	void OnCollision(const CollisionInfo& info)override;
	void OnInspect()override;
	void Configure(const nlohmann::json& data);
	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "JumpBlockComponent"; }
	bool OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene)override;
	void RequestStateChangeCommand();
	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }

	//セッター/ゲッター
	void SetTargetPos(const Math::Vector3& pos);
	const Math::Vector3 GetTargetPos()const;

	const Math::Vector3& GetJumpDirection()const { return m_jumpDirection; }
	void SetJumpDirection(const Math::Vector3& direction) { m_jumpDirection = direction; }

	const Math::Vector3& GetOwnerPos()const;

	const float& GetJumpForce()const { return m_jumpForce; }
	void SetJumpForce(const float& force) { m_jumpForce = force; }

private:
	std::shared_ptr<TransformComponent> m_transform;

	//パラメータ
	Math::Vector3 m_jumpDirection = Math::Vector3{ 0.0f,4.0f,0.0f }; //デフォルトは真上
	float m_jumpForce = 100.0f; //デフォルトのジャンプ力

	bool m_isTargetGizmoDragging = false;
	std::weak_ptr<GameViewModel> m_wpViewModel;
};