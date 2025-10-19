//#include "BoxColliderComponent.h"
//#include"../../GameObject.h"
//#include"../../TransformComponent/TransformComponent.h"
//
//DirectX::BoundingOrientedBox BoxColliderComponent::GetWorldOBB() const
//{
//	DirectX::BoundingOrientedBox obb{};
//	if (!m_owner)return obb;
//
//	auto transform = m_owner->GetComponent<TransformComponent>();
//	if (!transform)return obb;
//
//	obb.Center = transform->GetPos() + m_offset;
//	obb.Extents = m_extents * transform->GetScale();
//
//	Math::Quaternion q{};
//	q.CreateFromYawPitchRoll(
//		DirectX::XMConvertToRadians(transform->GetRot().y),
//		DirectX::XMConvertToRadians(transform->GetRot().x),
//		DirectX::XMConvertToRadians(transform->GetRot().z)
//	);
//	obb.Orientation = q;
//
//	return obb;
//}
