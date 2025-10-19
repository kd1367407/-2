#pragma once
#include"../Component.h"
#include"../ICollisionReceiver.h"

class TransformComponent;
class GameViewModel;

class ScalingBlockComponent:public Component,public ICollisionReceiver
{
public:
	void Configure(const nlohmann::json& data) override;
	void Awake()override;
	void Start() override;
	void Update() override;
	void OnInspect() override;
	void OnCollision(const CollisionInfo& info) override;

	const char* GetComponentName() const override { return "ScalingBlockComponent"; }

	void RequestTransformChangeCommand();

	const Math::Vector3& GetScaleAxis()const { return m_scaleAxis; }
	const float& GetscaleAmount()const { return m_scaleAmount; }
	const float& GetscaleSpeed()const { return m_scaleSpeed; }
	void SetScaleAxis(const Math::Vector3& axis) { m_scaleAxis = axis; }
	void SetScaleAmount(const float& amount) { m_scaleAmount = amount; }
	void SetScaleSpeed(const float& speed) { m_scaleSpeed = speed; }
	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }

private:
	std::weak_ptr<TransformComponent> m_wpTransform;
	std::weak_ptr<GameViewModel> m_wpViewModel;

	//--パラメータ--
	Math::Vector3 m_scaleAxis = { 1.0f,1.0f,1.0f };//拡縮軸
	float m_scaleAmount = 0.5f;//拡縮量
	float m_scaleSpeed = 1.0f;//拡縮スピード

	//--状態変数--
	Math::Vector3 m_baseScale;//初期スケール
	float m_elapsedTime = 0.0f;//経過時間
	Math::Vector3 m_lastScale; // 1フレーム前のスケールを記憶
	std::weak_ptr<GameObject> m_wpPlayer;//playerのポインタ
	bool m_isPlayerOnTop = false;//playerが上にいるかどうか
	float m_playerOffTimer = 0.0f;//クールタイム
	float m_colliderFullHeight = 0.0f;
};