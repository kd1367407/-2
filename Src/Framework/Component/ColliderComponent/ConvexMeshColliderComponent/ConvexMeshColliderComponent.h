//#pragma once
//#include"../ColliderComponent.h"
//
////動く動的なオブジェクトの当たり判定コンポーネント(凸メッシュ)
//class ConvexMeshColliderComponent:public ColliderComponent
//{
//public:
//	~ConvexMeshColliderComponent()override;
//	
//	ShapeType GetShapeType()const override { return ShapeType::Mesh; }
//
//	void Awake()override;
//	void Start()override;
//	void SetModel(const std::string& modelPath) { m_modelPath = modelPath; }
//
//private:
//	std::shared_ptr<KdModelData> m_spModel;
//	rp3d::ConvexMesh* m_convexMesh = nullptr;
//	rp3d::ConvexMeshShape* m_convexShape = nullptr;
//	rp3d::Collider* m_collider = nullptr;
//};