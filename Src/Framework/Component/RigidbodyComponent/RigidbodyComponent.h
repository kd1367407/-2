#pragma once
#include"../Component.h"

class TransformComponent;

// ボディタイプを定義
enum class RigidbodyType {
	Dynamic,   // プレイヤー
	Kinematic, // 動く床・壁
	Static     // 動かないもの
};

//動くオブジェクトであることを示すマーカー
class RigidbodyComponent:public Component, public std::enable_shared_from_this<RigidbodyComponent>
{
public:
	RigidbodyComponent() { m_updatePriority = 0; }//TransformComponentに依存するためTransformComponentの後に実行

	void Awake()override;
	void Start()override;

	void Configure(const nlohmann::json& data);
	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "RigidbodyComponent"; }

	void AddForce(const Math::Vector3& force) { m_force += force; }
	void SetVelocity(const Math::Vector3& velocity) { m_velocity = velocity; }
	void SetGround(const std::shared_ptr<GameObject>& groundObject);

	//IInspectableとして必須の実装
	void OnInspect()override;

	const std::shared_ptr<TransformComponent>& GetTransform()const { return m_transform; }

	//物理パラメータ
	Math::Vector3 m_velocity = Math::Vector3::Zero;
	Math::Vector3 m_force = Math::Vector3::Zero;
	float m_mass = 1.0f;
	RigidbodyType m_type = RigidbodyType::Dynamic;

	//乗っている床の情報
	std::weak_ptr<TransformComponent> m_groundTransform;
	Math::Vector3 m_groundLastPos = Math::Vector3::Zero;
	Math::Vector3 m_additionalMovement = Math::Vector3::Zero;

	bool m_isJumpRequested = false;
	Math::Vector3 m_jumpVelocity = Math::Vector3::Zero;

	bool m_isOnSlipperySurface = false;
	float m_surfaceDragValue = 0.0f;

private:
	//頻繫に使うためキャッシュしておく
	std::shared_ptr<TransformComponent> m_transform;
};