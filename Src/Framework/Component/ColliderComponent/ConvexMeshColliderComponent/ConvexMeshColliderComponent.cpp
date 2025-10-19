//#include "ConvexMeshColliderComponent.h"
//#include"../../GameObject.h"
//#include"../../TransformComponent/TransformComponent.h"
//#include"../../RigidbodyComponent/RigidbodyComponent.h"
//#include"../Src/Application/System/PhysicsSystem.h"
//#include"../Src/Application/main.h"
//
//ConvexMeshColliderComponent::~ConvexMeshColliderComponent()
//{
//	
//}
//
//void ConvexMeshColliderComponent::Awake()
//{
//	if (m_modelPath.empty())return;
//
//	m_spModel = KdAssets::Instance().m_modeldatas.GetData(m_modelPath);
//	if (!m_spModel)return;
//
//	auto physicsCommon = PhysicsSystem::Instance().GetCommon();
//
//	//凸メッシュは1つにまとめる必要があるため全頂点を一旦収集
//	std::vector<Math::Vector3> allVertices{};
//
//	const auto& nodes = m_spModel->GetOriginalNodes();
//	for (int nodeIdx : m_spModel->GetCollisionMeshNodeIndices())
//	{
//		const auto& mesh = nodes[nodeIdx].m_spMesh;
//		if (!mesh)continue;
//		const auto& vertices = mesh->GetVertexPositions();
//		allVertices.insert(allVertices.end(), vertices.begin(), vertices.end());
//	}
//
//	if (allVertices.empty())return;
//
//	rp3d::VertexArray vertexArray(
//		allVertices.data(),
//		sizeof(Math::Vector3),
//		(UINT)allVertices.size(),
//		rp3d::VertexArray::DataType::VERTEX_FLOAT_TYPE
//	);
//
//	std::vector<rp3d::Message> messages{};
//	m_convexMesh = physicsCommon->createConvexMesh(vertexArray, messages);
//
//	if (!m_convexMesh)
//	{
//		for (const auto& msg : messages)
//		{
//			Application::Instance().AddLog(msg.text.c_str());
//		}
//		return;
//	}
//
//	m_convexShape = physicsCommon->createConvexMeshShape(m_convexMesh);
//
//	std::vector<Math::Vector3> allVeritices;
//}
//
//void ConvexMeshColliderComponent::Start()
//{
//	auto rigidbodyComp = m_owner->GetComponent<RigidbodyComponent>();
//	if (!rigidbodyComp)return;
//	
//	rp3d::RigidBody* body = rigidbodyComp->GetBody();
//	if (!body || !m_convexShape)return;
//
//
//	//ColliderのローカルTransformを設定(scaleはここで反映)
//	auto transform = m_owner->GetComponent<TransformComponent>();
//	auto scale = transform->GetScale();
//	m_convexShape->setScale({ scale.x,scale.y,scale.z });
//
//	m_collider = body->addCollider(m_convexShape, rp3d::Transform::identity());
//}
