//#include "MeshColliderComponent.h"
//#include"../../GameObject.h"
//#include"../../TransformComponent/TransformComponent.h"
//#include"../Src/Application/System/PhysicsSystem.h"
//#include"../Src/Application/main.h"
//
//MeshColliderComponent::~MeshColliderComponent()
//{
//}
//
//void MeshColliderComponent::Awake()
//{
//	if (m_modelPath.empty())return;
//
//	m_spModel = KdAssets::Instance().m_modeldatas.GetData(m_modelPath);
//	if (!m_spModel)return;
//
//	auto physicsCommon = PhysicsSystem::Instance().GetCommon();
//
//	//複数の"COL"メッシュを1つのデータに統合する
//	std::vector<Math::Vector3> combinedVertices;
//	std::vector<UINT> combinedIndices;
//	UINT indexOffset = 0;
//
//	const auto& nodes = m_spModel->GetOriginalNodes();
//	for (int nodeIdx : m_spModel->GetCollisionMeshNodeIndices())
//	{
//		const auto& mesh = nodes[nodeIdx].m_spMesh;
//		if (!mesh)continue;
//
//		const auto& vertices = mesh->GetVertexPositions();
//		const auto& faces = mesh->GetFaces();
//
//		//頂点をコピー
//		combinedVertices.insert(combinedVertices.end(), vertices.begin(), vertices.end());
//
//		//インデックスをコピーし、オフセットを追加
//		for (const auto& face : faces)
//		{
//			combinedIndices.push_back(face.Idx[0] + indexOffset);
//			combinedIndices.push_back(face.Idx[1] + indexOffset);
//			combinedIndices.push_back(face.Idx[2] + indexOffset);
//		}
//
//		indexOffset += (UINT)vertices.size();
//	}
//
//	if (combinedVertices.empty() || combinedIndices.empty())return;
//
//	//結合したデータを使ってReactPhysics3Dオブジェクトを生成
//
//	//TriangleVertexArrayを作成
//	rp3d::TriangleVertexArray triangleArray(
//		(UINT)combinedVertices.size(),//頂点数
//		combinedVertices.data(),//頂点データ配列の先頭ポインタ
//		sizeof(Math::Vector3),//頂点1つあたりのバイト数(ストライド)
//		(UINT)combinedIndices.size() / 3,//面(三角形)の数
//		combinedIndices.data(),//インデックスデータ配列の先頭ポインタ
//		3 * sizeof(UINT),//1つの面のバイト数
//		rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
//		rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
//	);
//
//	//TriangleMeshを作成
//	std::vector<rp3d::Message> messages;//エラーメッセージ受け取り用
//	m_triangleMesh = physicsCommon->createTriangleMesh(triangleArray, messages);
//
//	//エラーがあれば表示
//	for (const auto& msg : messages)
//	{
//		Application::Instance().AddLog(msg.text.c_str());
//	}
//
//	if (!m_triangleMesh)return;
//
//	//ConcaveMeshShapeを作成
//	//これが最終的な当たり判定形状
//	m_concaveShape = physicsCommon->createConcaveMeshShape(m_triangleMesh);
//}
//
//void MeshColliderComponent::Start()
//{
//	auto transform = m_owner->GetComponent<TransformComponent>();
//	if (!transform || !m_concaveShape)return;
//
//	//静的なRigidbodyを生成
//	rp3d::Transform rp3dTransform = rp3d::Transform::identity();//位置、回転はColliderで設定
//	rp3d::RigidBody* body = PhysicsSystem::Instance().GetWorld()->createRigidBody(rp3dTransform);
//	body->setType(rp3d::BodyType::STATIC);//動かないオブジェクトとして設定
//
//	//ColliderをRigidbodyに追加
//	//ColliderのローカルTransformとしてGameObjectのTransformを設定
//	auto pos = transform->GetPos();
//	auto rot = transform->GetRot();
//	auto dxQuat = DirectX::XMQuaternionRotationRollPitchYaw(
//		DirectX::XMConvertToRadians(rot.x),
//		DirectX::XMConvertToRadians(rot.y),
//		DirectX::XMConvertToRadians(rot.z)
//	);
//	rp3d::Quaternion rp3dQuat(DirectX::XMVectorGetX(dxQuat), DirectX::XMVectorGetY(dxQuat), DirectX::XMVectorGetZ(dxQuat), DirectX::XMVectorGetW(dxQuat));
//	rp3d::Transform colliderTransform({ pos.x,pos.y,pos.z }, rp3dQuat);
//
//	m_collider = body->addCollider(m_concaveShape, colliderTransform);
//}
