//#include "PhysicsDebugDrawerComponent.h"
//#include"../Src/Application/System/PhysicsSystem.h"
//
//void PhysicsDebugDrawerComponent::Awake()
//{
//	//PhysicsWorldのデバッグ表示機能を有効化
//	auto* world = PhysicsSystem::Instance().GetWorld();
//	if (world)
//	{
//		world->setIsDebugRenderingEnabled(true);
//		//表示したい当たり判定の種類を設定
//		world->getDebugRenderer().setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
//		world->getDebugRenderer().setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
//	}
//}
//
//void PhysicsDebugDrawerComponent::PostUpdate()
//{
//	m_debugVertices.clear();
//
//	auto* world = PhysicsSystem::Instance().GetWorld();
//	if (!world || !world->getIsDebugRenderingEnabled())return;
//
//	auto& debugRenderer = world->getDebugRenderer();
//
//	//描画用の線の数を取得
//	UINT32 numLines = debugRenderer.getNbLines();
//	if (numLines > 0)
//	{
//		m_debugVertices.resize(numLines * 2);//1本の線は2個の頂点からなる
//		const rp3d::DebugRenderer::DebugLine* lines = debugRenderer.getLinesArray();
//		for (UINT32 i = 0; i < numLines; ++i)
//		{
//			KdPolygon::Vertex v1, v2;
//
//			// 始点
//			v1.pos = { lines[i].point1.x, lines[i].point1.y, lines[i].point1.z };
//			v1.color = lines[i].color1; // ColorはUINT型なので直接代入
//			v1.UV = { 0, 0 };
//
//			// 終点
//			v2.pos = { lines[i].point2.x, lines[i].point2.y, lines[i].point2.z };
//			v2.color = lines[i].color2;
//			v2.UV = { 0, 0 };
//
//			m_debugVertices.push_back(v1);
//			m_debugVertices.push_back(v2);
//		}
//	}
//}
//
//void PhysicsDebugDrawerComponent::DrawDebug()
//{
//	if (m_debugVertices.empty())return;
//
//	KdDirect3D::Instance().DrawVertices(
//		D3D_PRIMITIVE_TOPOLOGY_LINELIST,
//		m_debugVertices.size(),
//		m_debugVertices.data(),
//		sizeof(KdPolygon::Vertex)
//	);
//}
