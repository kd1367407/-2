//#include "SphereColliderComponent.h"
//#include"../../GameObject.h"
//#include"../../RigidbodyComponent/RigidbodyComponent.h"
//#include"../Src/Application/System/PhysicsSystem.h"
//
//
//SphereColliderComponent::~SphereColliderComponent()
//{
//	
//}
//
//void SphereColliderComponent::Awake()
//{
//	auto physicsCommon = PhysicsSystem::Instance().GetCommon();
//	m_shape = physicsCommon->createSphereShape(m_radius);
//}
//
//void SphereColliderComponent::Start()
//{
//	auto rigidbodyComp = m_owner->GetComponent<RigidbodyComponent>();
//	if (!rigidbodyComp)
//	{
//		// Rigidbodyがない場合は静的オブジェクトとして生成するなど、今後の拡張も可能
//		return;
//	}
//
//	rp3d::RigidBody* body = rigidbodyComp->GetBody();
//	if (!body)return;
//
//	//コライダーをRigidbodyに追加
//	m_collider = body->addCollider(m_shape, rp3d::Transform::identity());
//}
