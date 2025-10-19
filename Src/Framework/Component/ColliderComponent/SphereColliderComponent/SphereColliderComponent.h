//#pragma once
//#include"../ColliderComponent.h"
//
//
//class SphereColliderComponent:public ColliderComponent
//{
//public:
//	~SphereColliderComponent()override;
//	
//	void Awake()override;
//	void Start()override;
//
//	float GetRadius()const { return m_radius; }
//	void SetRadius(float radius) { m_radius = radius; }
//
//private:
//	float m_radius = 0.5f;
//	rp3d::SphereShape* m_shape = nullptr;
//	rp3d::Collider* m_collider = nullptr;
//};