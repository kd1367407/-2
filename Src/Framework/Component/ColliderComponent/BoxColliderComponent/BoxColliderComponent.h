//#pragma once
//#include"../ColliderComponent.h"
//
//class BoxColliderComponent:public ColliderComponent
//{
//public:
//	ShapeType GetShapeType()const override { return ShapeType::Box; }
//
//	//Extentsは中心から各面までの距離(サイズ/2)
//	const Math::Vector3& GetExtents()const { return m_extents; }
//	void SetExtents(Math::Vector3& extents) { m_extents = extents; }
//
//	//ワールド座標でのBoundingOrientedBoxを取得
//	DirectX::BoundingOrientedBox GetWorldOBB()const;
//
//private:
//	Math::Vector3 m_extents = { 0.5f,0.5f,0.5f };
//};