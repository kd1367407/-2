#pragma once
#include"../Src/Framework/Component/ColliderComponent/ColliderComponent.h"//Layerのため

class GameObject;
class RigidbodyComponent;
class ColliderComponent;
class TransformComponent;

//レイキャストの結果を格納する構造体
struct RayResult
{
	std::weak_ptr<GameObject> m_hitObject;//ヒットしたGameObject
	UINT m_hitObjectId = 0;//ヒットしたGameObjectのID
	Math::Vector3 m_hitPos;//衝突点
	Math::Vector3 m_hitNormal;//衝突点の法線
	float m_distance = 0.0f;//レイの始点からの距離
};

//レイキャストの入力情報
struct RayInfo
{
	Math::Vector3 m_start;//始点
	Math::Vector3 m_dir;//方向(正規化前提)
	float m_maxDistance = 1000.0f;//最大距離
};

//物理演算システム
class PhysicsSystem
{
public:
	static PhysicsSystem& Instance();
	~PhysicsSystem() { m_colliders.clear(); }

	void RegisterCollider(const std::shared_ptr<ColliderComponent>& collider);
	void RegisterRigidbody(const std::shared_ptr<RigidbodyComponent>& rigid);

	//物理演算と衝突解決
	void Update(float deltatime);

	//レイキャスト実行
	bool Raycast(const RayInfo& rayInfo, RayResult& outResult, uint32_t layerMask = LayerAll, const GameObject* ignoreObject = nullptr);
	bool RaycastToDebug(const RayInfo& rayInfo, RayResult& outResult, uint32_t layerMask = LayerAll, const GameObject* ignoreObject = nullptr);

	//デバッグ描画
	void DrawDebug(class KdDebugWireFrame& wire);

	//void RegisterJumpDebugInfo(const Math::Vector3& startPos,const Math::)

private:
	PhysicsSystem() = default;
	
	PhysicsSystem(const PhysicsSystem&) = delete;
	PhysicsSystem& operator=(const PhysicsSystem&) = delete;

	//衝突解決解決ヘルパー
	void ResolveCoolision(const std::shared_ptr<ColliderComponent>& colA, const std::shared_ptr<ColliderComponent>& colB);

	//押し出し処理
	void Pushout(GameObject* dynamicObj, RigidbodyComponent* rigid, Shape* dynamicShape, GameObject* staticObj, Shape* staticShape);

	bool CheckSpherePolygonIntersection(const DirectX::BoundingSphere& spahe, PolygonShape* polygon, const Math::Matrix& mat);

	bool CalculateSpherePolygonPushout(const DirectX::BoundingSphere& shape, PolygonShape* polygon, const Math::Matrix& mat, CollisionMeshResult* result);

	//登録された全コライダーのリスト
	std::vector<std::weak_ptr<ColliderComponent>> m_colliders;

	std::vector<std::weak_ptr<RigidbodyComponent>> m_rigidbodies;

	//レイデバッグ
	RayInfo m_debugRay;
	bool m_shouldDrawDebugRay = false;
};