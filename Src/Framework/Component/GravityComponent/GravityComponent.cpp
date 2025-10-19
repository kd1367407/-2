#include "GravityComponent.h"
#include"../GameObject.h"
#include"../RigidbodyComponent/RigidbodyComponent.h"
#include"../Src/Application/System/PhysicsSystem.h"
#include"../TransformComponent/TransformComponent.h"
#include"../ColliderComponent/ColliderComponent.h"
#include"../PlayerStatsComponent/PlayerStatsComponent.h"
#include"../Src/Application/main.h"
#include"../Src/Application/SettingsManager/SettingsManager.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void GravityComponent::Awake()
{
	const auto& settings = SettingsManager::Instance().GetGameSetting();
	const auto& worldSettings = settings["world_settings"];
	m_gameGravity = JsonHelper::GetFloat(worldSettings, "gravity", 20);
	m_fallTimeLimit = JsonHelper::GetFloat(worldSettings, "fall_time_limit", 5.0f);
	m_isOnGround = false;
}

void GravityComponent::Start()
{
	m_rigidbody = m_owner->GetComponent<RigidbodyComponent>();
}

void GravityComponent::Update()
{
	if (!m_rigidbody)return;

	if (!m_isOnGround)
	{
		Math::Vector3 gravityForce(0.0, -m_gameGravity, 0.0);
		m_rigidbody->AddForce(gravityForce);
	}
}

void GravityComponent::PostUpdate()
{
	//地面判定
	CheckGround();

	if (m_isOnGround)
	{
		m_fallTimer = 0.0f;
	}
	else
	{
		float deltatime = Application::Instance().GetDeltaTime();
		m_fallTimer += deltatime;

		if (m_fallTimer >= m_fallTimeLimit)
		{
			if (auto stats = m_owner->GetComponent<PlayerStatsComponent>())
			{
				if (auto transform = m_owner->GetComponent<TransformComponent>())
				{
					transform->SetPos(stats->GetInitialPos());
					if (m_rigidbody)
					{
						m_rigidbody->SetVelocity(Math::Vector3::Zero);
						m_rigidbody->m_force = Math::Vector3::Zero;
					}
					m_fallTimer = 0.0f;
					Application::Instance().AddLog("Player has fallen and been reset.");
					return;
				}
			}
		}
	}
}

void GravityComponent::CheckGround()
{
	auto transform = m_owner->GetComponent<TransformComponent>();
	if (!transform)
	{
		m_isOnGround = false;
		return;
	}

	auto collider = m_owner->GetComponent<ColliderComponent>();
	if (!collider)
	{
		m_isOnGround = false;
		return;
	}

	Shape* shape = collider->GetShape();
	if (!shape || shape->GetType() != Shape::Type::Sphere)
	{
		m_isOnGround = false;
		return;
	}

	auto* sphereShape = static_cast<SphereShape*>(shape);

	RayInfo ray;
	ray.m_start = transform->GetPos();
	ray.m_dir = { 0,-1,0 };
	// 少し長めにレイを飛ばして、坂道などでも安定して接地するように調整
	ray.m_maxDistance = sphereShape->m_radius + 0.2f;

	RayResult result;
	if (PhysicsSystem::Instance().Raycast(ray, result, LayerAll, m_owner))
	{
		// 地面との距離が一定以下なら接地していると判断
		if (result.m_distance <= (sphereShape->m_radius + 0.2f))
		{
			m_isOnGround = true;
			m_rigidbody->SetGround(result.m_hitObject.lock());
			return;
		}
	}

	m_isOnGround = false;
	m_rigidbody->SetGround(nullptr);
}

