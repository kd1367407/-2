//#pragma once
//#include"../ColliderComponent.h"
//
////動かない静的なオブジェクトの当たり判定コンポーネント(凹メッシュ)
//class MeshColliderComponent:public ColliderComponent
//{
//public:
//	~MeshColliderComponent()override;
//
//	ShapeType GetShapeType()const override { return ShapeType::Mesh; }
//
//	void Awake()override;
//	void Start()override;
//
//	//モデルデータをセットし、当たり判定用のメッシュを準備
//	void SetModel(const std::string& modelPath) { m_modelPath = modelPath; }
//
//private:
//	std::shared_ptr<KdModelData> m_spModel;
//
//	//ReactPhysics3D用のオブジェクト
//	rp3d::TriangleMesh* m_triangleMesh = nullptr;
//	rp3d::ConcaveMeshShape* m_concaveShape = nullptr;
//	rp3d::Collider* m_collider = nullptr;
//
//	//複数のメッシュをまとめるためのデータ
//	std::vector<rp3d::TriangleVertexArray*> m_vertexArrays;
//};