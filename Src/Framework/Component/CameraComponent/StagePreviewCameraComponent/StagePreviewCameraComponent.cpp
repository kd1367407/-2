#include "StagePreviewCameraComponent.h"
#include"../../TransformComponent/TransformComponent.h"
#include"../../GameObject.h"

void StagePreviewCameraComponent::AdjustCameraToShowAll(const std::list<std::shared_ptr<GameObject>>& objects)
{
	if (objects.empty())return;

	//シーン全体のboundingboxを計算
	DirectX::BoundingBox sceneBounds;
	bool first = true;
	for (const auto& obj : objects)
	{
		if (auto transform = obj->GetComponent<TransformComponent>())
		{
			DirectX::BoundingBox objBounds(transform->GetPos(), Math::Vector3::Zero);
			if (first)
			{
				sceneBounds = objBounds;
				first = false;
			}
			else
			{
				DirectX::BoundingBox::CreateMerged(sceneBounds, sceneBounds, objBounds);
			}
		}
	}

	//boundingboxを収めるためのカメラ位置計算
	float distance = (sceneBounds.Extents.x > sceneBounds.Extents.y ? sceneBounds.Extents.x : sceneBounds.Extents.y) * 2.5f;
	distance += sceneBounds.Extents.z;

	Math::Vector3 cameraPos = sceneBounds.Center;
	cameraPos.z -= distance;
	cameraPos.y += distance * 0.5f;//俯瞰させる感じ

	if (auto trans = m_owner->GetComponent<TransformComponent>())
	{
		trans->SetPos(cameraPos);
		m_spCamera->SetLookAt(sceneBounds.Center);
	}
}
