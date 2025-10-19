#include "CheckpointComponent.h"
#include"../GameObject.h"
#include"../PlayerStatsComponent/PlayerStatsComponent.h"
#include"../ColliderComponent/ColliderComponent.h"
#include"../TransformComponent/TransformComponent.h"

void CheckpointComponent::Awake()
{
	int a = 0;
}

void CheckpointComponent::Start()
{
}

void CheckpointComponent::OnCollision(const CollisionInfo& info)
{
	if (!info.otherObject)return;

	if (auto playerStats = info.otherObject->GetComponent<PlayerStatsComponent>())
	{
		if (info.contactNormal.y > 0.7f)
		{
			//このブロックの上面を計算
			auto selfTransform = m_owner->GetComponent<TransformComponent>();
			auto selfCollider = m_owner->GetComponent<ColliderComponent>();
			if (!selfTransform || !selfCollider || !selfCollider->GetShape()) return;

			const auto& aabb = selfCollider->GetShape()->GetBoundingBox();
			Math::Vector3 newRespawnPos = selfTransform->GetPos();
			newRespawnPos.y += aabb.Center.y + aabb.Extents.y;

			//リスポーン地点更新
			playerStats->SetInitialPos(newRespawnPos);
		}
	}
}
