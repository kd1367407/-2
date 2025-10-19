#include "PhysicsSystem.h"
#include"../Src/Framework/Component/GameObject.h"//Raycastで使用
#include"../Src/Framework/Component/ColliderComponent/ColliderComponent.h"
#include"../Src/Framework/Component/RigidbodyComponent/RigidbodyComponent.h"
#include"../Src/Framework/Component/TransformComponent/TransformComponent.h"
#include"../Src/Framework/Component/ColliderComponent/Shape.h"
#include"../Src/Framework/Component/TimerComponent/TimerComponent.h"
#include"../main.h"
#include"../Scene/SceneManager.h"
#include"../Scene/BaseScene/BaseScene.h"
#include"../Scene/GameScene/GameManager/GameManager.h"
#include"../Src/Framework/Component/ICollisionReceiver.h"
#include"../Src/Framework/Component/GravityComponent/GravityComponent.h"
#include"../Src/Framework/Math/KdCollision.h"
#include"../Src/Framework/Component/ColliderComponent/CollisionUtils/CollisionUtils.h"
#include"../Src/Framework/Component/ITriggerReceiver.h"

static bool CalculatePolygonSpherePushout(
	const PolygonShape* polygon,
	const DirectX::BoundingSphere& sphere,
	const DirectX::XMMATRIX& matrix,
	CollisionMeshResult* pResult)
{
	//--ブロードフェイズ--
	DirectX::BoundingBox aabb;
	polygon->GetBoundingBox().Transform(aabb, matrix);
	if (!aabb.Intersects(sphere)) { return false; }

	//--ナローフェイズ--
	bool isHit = false;
	const auto& vertices = polygon->GetVertices();
	const auto& faces = polygon->GetFaces();

	//判定に必要な変数を準備
	DirectX::XMVECTOR finalHitPos = {};// 最終的な接触点（ローカル座標）
	DirectX::XMVECTOR finalPos = {};// 押し出された後の球の中心（ローカル座標）
	DirectX::XMVECTOR objScale = {};// オブジェクトのスケール
	std::vector<Math::Vector3> finalFace;//最終的な面
	float radiusSqr = 0.0f;// 球の半径の2乗

	//球の情報をローカル座標系に変換
	InvertSphereInfo(finalPos, objScale, radiusSqr, matrix, sphere);

	const DirectX::XMVECTOR initialPos = finalPos;

	//全ての面と反復的に当たり判定
	for (const auto& face : faces)
	{
		DirectX::XMVECTOR nearPoint;
		const UINT* idx = face.Idx;

		//現在の球の中心(finalPos)と現在の三角形との最近接点を求める
		KdPointToTriangle(finalPos, vertices[idx[0]], vertices[idx[1]], vertices[idx[2]], nearPoint);

		std::vector<Math::Vector3> currentFaceVerts = { vertices[idx[0]], vertices[idx[1]], vertices[idx[2]] };

		//接触判定と球の中心位置(finalPos)の更新
		isHit |= HitCheckAndPosUpdate(finalPos, finalHitPos, finalFace, currentFaceVerts, nearPoint, objScale, radiusSqr, sphere.Radius);
	}

	//--結果の格納--
	if (pResult && isHit)
	{
		SetIterativePushoutResult(*pResult, isHit, initialPos, finalPos, matrix);
	}

	return isHit;
}

PhysicsSystem& PhysicsSystem::Instance()
{
	static PhysicsSystem instance;
	return instance;
}

void PhysicsSystem::RegisterCollider(const std::shared_ptr<ColliderComponent>& collider)
{
	m_colliders.push_back(collider);
}

void PhysicsSystem::RegisterRigidbody(const std::shared_ptr<RigidbodyComponent>& rigid)
{
	m_rigidbodies.push_back(rigid);
}

void PhysicsSystem::Update(float deltatime)
{
	//rigidbodyを持つオブジェクトを物理法則に従って動かす
	for (auto& weakRigid : m_rigidbodies)
	{
		if (auto spRigid = weakRigid.lock())
		{
			auto transform = spRigid->GetTransform();
			if (!transform)continue;

			//ジャンプリクエストがあれば速度を強制的に設定
			if (spRigid->m_isJumpRequested)
			{
				spRigid->m_velocity = spRigid->m_jumpVelocity;
				spRigid->m_isJumpRequested = false;
			}

			//動く床の移動量を計算して適用
			Math::Vector3 platformMovement = Math::Vector3::Zero;
			if (auto ground = spRigid->m_groundTransform.lock())
			{
				platformMovement = ground->GetPos() - spRigid->m_groundLastPos;
				spRigid->m_groundLastPos = ground->GetPos();
			}

			//自身の物理演算(Dynamicのみ)
			if (spRigid->m_type == RigidbodyType::Dynamic)
			{
				//空気抵抗を加える
				const float defaultDragCoefficient = 1.5f;
				float dragCoefficient = defaultDragCoefficient;

				if (spRigid->m_isOnSlipperySurface)
				{
					dragCoefficient = spRigid->m_surfaceDragValue;
				}

				Math::Vector3 horizontalVelocity = spRigid->m_velocity;
				horizontalVelocity.y = 0; // Y軸（垂直方向）の速度を無視
				spRigid->AddForce(-horizontalVelocity * dragCoefficient);

				//物理法則に従って速度と位置を更新
				Math::Vector3 acceleration = spRigid->m_force / spRigid->m_mass;
				spRigid->m_velocity += acceleration * deltatime;

				spRigid->m_force = Math::Vector3::Zero;
			}

			//最終的な移動を実行
			transform->Move(spRigid->m_velocity * deltatime + platformMovement + spRigid->m_additionalMovement);

			//次のフレームのためにリセット
			spRigid->m_additionalMovement = Math::Vector3::Zero;
		}
	}

	//衝突解決
	for (size_t i = 0; i < m_colliders.size(); ++i)
	{
		for (size_t j = i + 1; j < m_colliders.size(); ++j)
		{
			if (auto colA = m_colliders[i].lock())
			{
				if (auto colB = m_colliders[j].lock())
				{
					ResolveCoolision(colA, colB);
				}
			}
		}
	}
}

bool PhysicsSystem::Raycast(const RayInfo& rayInfo, RayResult& outResult, uint32_t layerMask, const GameObject* ignoreObject)
{
	bool hit = false;
	float closestDist = FLT_MAX;
	RayResult finalResult;

	//登録されている全てのコライダーと判定
	for (auto& weakCollider : m_colliders)
	{
		if (auto spCollider = weakCollider.lock())
		{
			if (!(spCollider->GetLayer() & layerMask))
			{
				continue;
			}

			auto owner = spCollider->GetOwner();

			//無視するオブジェクトと一致したらスキップ
			if (owner == ignoreObject)
			{
				continue;
			}

			auto transform = spCollider->GetTransform();
			Shape* shape = spCollider->GetShape();
			if (!transform || !shape)continue;

			float hitDist = -1.0f;
			Math::Vector3 hitPos{}, hitNolmal{};
			bool bThisShapeHit = false;//この形状でヒットしたかどうか

			//形状ごとの当たり判定
			switch (shape->GetType())
			{
				case Shape::Type::Sphere:
				{
					auto* sphereShape = static_cast<SphereShape*>(shape);
					DirectX::BoundingSphere sphere(transform->GetPos() + sphereShape->m_offset, sphereShape->m_radius);
					if (sphere.Intersects(rayInfo.m_start, rayInfo.m_dir, hitDist))
					{
						bThisShapeHit = true;
						hitPos = rayInfo.m_start + rayInfo.m_dir * hitDist;
						hitNolmal = hitPos - sphere.Center;
						hitNolmal.Normalize();
					}
					break;
				}
				case Shape::Type::Box:
				{
					auto* boxShape = static_cast<BoxShape*>(shape);
					DirectX::BoundingOrientedBox obb;
					obb.Center = transform->GetPos() + boxShape->m_offset;
					obb.Extents = boxShape->m_extents;
					Math::Quaternion q;
					q.CreateFromYawPitchRoll(
						DirectX::XMConvertToRadians(transform->GetRot().y),
						DirectX::XMConvertToRadians(transform->GetRot().x),
						DirectX::XMConvertToRadians(transform->GetRot().z)
					);
					obb.Orientation = q;
					if (obb.Intersects(rayInfo.m_start, rayInfo.m_dir, hitDist))
					{
						bThisShapeHit = true;
						hitPos = rayInfo.m_start + rayInfo.m_dir * hitDist;
						//簡易的法線
						hitNolmal = -rayInfo.m_dir;
					}
					break;
				}
				case Shape::Type::Mesh:
				{
					auto* meshShape = static_cast<MeshShape*>(shape);
					auto model = meshShape->m_spModel;

					if (model && !model->GetCollisionMeshNodeIndices().empty())
					{
						//当たり判定ように指定された全てのメッシュに対してチェック
						for (int nodeIndex : model->GetCollisionMeshNodeIndices())
						{
							//メッシュのポインタが有効かチェック
							if (const auto& mesh = model->GetMesh(nodeIndex))
							{
								CollisionMeshResult result;
								//ループで取得した正しいメッシュ(*mesh)を使って判定
								if (MeshIntersect(*mesh, rayInfo.m_start, rayInfo.m_dir, rayInfo.m_maxDistance, transform->GetMatrix(), &result))
								{
									//複数のメッシュにヒットした場合、最も近いものを採用(レイの最大距離 - 交点までの距離 で計算されるため、交点が近いほど、m_overlapDistanceは大きくなる)
									if (result.m_overlapDistance > hitDist || hitDist < 0)
									{
										bThisShapeHit = true;
										hitDist = rayInfo.m_maxDistance - result.m_overlapDistance;
										hitPos = result.m_hitPos;
										hitNolmal = result.m_hitNDir;
									}
								}
							}
						}
					}
					break;
				}
				case Shape::Type::Polygon:
				{
					//polygonShapeとして形状を取得
					auto* polygonShape = static_cast<PolygonShape*>(shape);
					if (!polygonShape)break;

					//--PolygonsIntersectの計算を流用--
					//レイをローカル座標系に(ポリゴンがローカル座標系だから)
					DirectX::XMVECTOR rayPosInv, rayDirInv;
					float rayRangeInv = 0.0f, scaleInv = 0.0f;
					InvertRayInfo(rayPosInv, rayDirInv, rayRangeInv, scaleInv, transform->GetMatrix(), rayInfo.m_start, rayInfo.m_dir, rayInfo.m_maxDistance);

					//頂点リストを取得
					const auto& positions = polygonShape->GetVertices();
					const auto& faces = polygonShape->GetFaces();
					float localClosestDist = FLT_MAX;
					size_t hitFaceIndex = -1;
					bool bHitInLoop = false;

					//全ての三角形とループで判定
					for (size_t faceid = 0; faceid < faces.size(); ++faceid)
					{
						const KdMeshFace& face = faces[faceid];
						const UINT* idx = face.Idx;

						float dist = FLT_MAX;
						if (DirectX::TriangleTests::Intersects(rayPosInv, rayDirInv, positions[idx[0]], positions[idx[1]], positions[idx[2]], dist))
						{
							if (dist > rayRangeInv)continue;//レイの範囲外なら無視

							if (dist < localClosestDist)
							{
								bHitInLoop = true;
								localClosestDist = dist;
								hitFaceIndex = faceid;
							}
						}
					}

					//ループ内で一度でも当たっていれば最終的な結果を計算(SetRayResultを参考)
					if (bHitInLoop)
					{
						CollisionMeshResult tempResult;
						tempResult.m_hit = true;

						//距離をワールド座標系に戻して計算(localClosestDistはスケールを考慮)
						tempResult.m_hitPos = rayInfo.m_start + rayInfo.m_dir * (localClosestDist / scaleInv);
						tempResult.m_overlapDistance = rayInfo.m_maxDistance - (localClosestDist / scaleInv);
						if (hitFaceIndex != -1)
						{
							const KdMeshFace& hitFace = faces[hitFaceIndex];
							const UINT* idx = hitFace.Idx;

							const Math::Vector3& v0 = positions[idx[0]];
							const Math::Vector3& v1 = positions[idx[1]];
							const Math::Vector3& v2 = positions[idx[2]];

							//外積を求めて法線の方向ベクトルを決定
							Math::Vector3 n = (v1 - v0).Cross(v2 - v0);
							//法線(ローカル座標系)にする
							n.Normalize();

							//ワールド座標系に戻す
							tempResult.m_hitNDir = DirectX::XMVector3TransformNormal(n, transform->GetMatrix());
						}

						if (tempResult.m_overlapDistance > hitDist || hitDist < 0)
						{
							bThisShapeHit = true;
							hitDist = rayInfo.m_maxDistance - tempResult.m_overlapDistance;
							hitPos = tempResult.m_hitPos;
							hitNolmal = tempResult.m_hitNDir;
						}
					}
					break;
				}
			}
			//より近いオブジェクトが見つかったかチェック
			if (bThisShapeHit && hitDist < closestDist)
			{
				hit = true;
				closestDist = hitDist;
				finalResult.m_hitObject = owner->shared_from_this();
				finalResult.m_distance = hitDist;
				finalResult.m_hitPos = hitPos;
				finalResult.m_hitNormal = hitNolmal;
			}
		}
	}
	if (hit)
	{
		outResult = finalResult;
	}
	return hit;
}

bool PhysicsSystem::RaycastToDebug(const RayInfo& rayInfo, RayResult& outResult, uint32_t layerMask, const GameObject* ignoreObject)
{
	m_debugRay = rayInfo;
	m_shouldDrawDebugRay = true;

	return Raycast(rayInfo, outResult, layerMask, ignoreObject);
}

void PhysicsSystem::DrawDebug(KdDebugWireFrame& wire)
{
	//登録されている全てのコライダーの形状を描画
	for (const auto& weakCol : m_colliders)
	{
		if (auto spCol = weakCol.lock())
		{
			auto transform = spCol->GetTransform();
			Shape* shape = spCol->GetShape();
			if (!transform || !shape)continue;

			//形状のオフセットを考慮した中心座標
			Math::Vector3 centerPos = transform->GetPos() + shape->m_offset;

			switch (shape->GetType())
			{
				case Shape::Type::Sphere:
				{
					auto* sphere = static_cast<SphereShape*>(shape);
					wire.AddDebugSphere(centerPos, sphere->m_radius, kGreenColor);
					break;
				}
				case Shape::Type::Box:
				{
					auto* box = static_cast<BoxShape*>(shape);
					wire.AddDebugBox(transform->GetMatrix(), box->m_extents, box->m_offset, true, kRedColor);
					break;
				}
			}
		}
	}

	if (m_shouldDrawDebugRay)
	{
		wire.AddDebugLine(m_debugRay.m_start, m_debugRay.m_dir, m_debugRay.m_maxDistance,kRedColor);
	}
}

void PhysicsSystem::ResolveCoolision(const std::shared_ptr<ColliderComponent>& colA, const std::shared_ptr<ColliderComponent>& colB)
{
	auto ownerA = colA->GetOwner();
	auto ownerB = colB->GetOwner();

	Shape* shapeA = colA->GetShape();
	Shape* shapeB = colB->GetShape();
	if (!shapeA || !shapeB)return;

	//実際に衝突しているかどうかを判定
	bool isColliding = false;
	auto transformA = ownerA->GetComponent<TransformComponent>();
	auto transformB = ownerB->GetComponent<TransformComponent>();
	if (!transformA || !transformB)return;

	//球 vs 箱
	if (shapeA->GetType() == Shape::Type::Sphere && shapeB->GetType() == Shape::Type::Box)
	{
		auto* sphereShape = static_cast<SphereShape*>(shapeA);
		auto* boxShape = static_cast<BoxShape*>(shapeB);
		DirectX::BoundingSphere sphere(transformA->GetPos() + sphereShape->m_offset, sphereShape->m_radius);
		DirectX::BoundingOrientedBox obb;
		obb.Center = transformB->GetPos() + boxShape->m_offset;
		obb.Extents = boxShape->m_extents;
		Math::Quaternion q;
		q.CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(transformB->GetRot().y),
			DirectX::XMConvertToRadians(transformB->GetRot().x),
			DirectX::XMConvertToRadians(transformB->GetRot().z)
		);
		obb.Orientation = q;
		isColliding = sphere.Intersects(obb);
	}
	// 箱 vs 球
	else if (shapeA->GetType() == Shape::Type::Box && shapeB->GetType() == Shape::Type::Sphere)
	{
		auto* boxShape = static_cast<BoxShape*>(shapeA);
		auto* sphereShape = static_cast<SphereShape*>(shapeB);
		DirectX::BoundingSphere sphere(transformB->GetPos() + sphereShape->m_offset, sphereShape->m_radius);
		DirectX::BoundingOrientedBox obb;
		obb.Center = transformA->GetPos() + boxShape->m_offset;
		obb.Extents = boxShape->m_extents;
		Math::Quaternion q;
		q.CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(transformA->GetRot().y),
			DirectX::XMConvertToRadians(transformA->GetRot().x),
			DirectX::XMConvertToRadians(transformA->GetRot().z)
		);
		obb.Orientation = q;
		isColliding = sphere.Intersects(obb);
	}
	//球 vs メッシュ
	else if (shapeA->GetType() == Shape::Type::Sphere && shapeB->GetType() == Shape::Type::Mesh)
	{
		auto* sphereShape = static_cast<SphereShape*>(shapeA);
		auto* meshShape = static_cast<MeshShape*>(shapeB);
		DirectX::BoundingSphere sphere(transformA->GetPos() + sphereShape->m_offset, sphereShape->m_radius);
		CollisionMeshResult result; // Pushoutに渡さないので結果は捨てる
		isColliding = MeshIntersect(*meshShape->m_spModel->GetMesh(0), sphere, transformB->GetMatrix(), &result);
	}
	// メッシュ vs 球
	else if (shapeA->GetType() == Shape::Type::Mesh && shapeB->GetType() == Shape::Type::Sphere)
	{
		auto* meshShape = static_cast<MeshShape*>(shapeA);
		auto* sphereShape = static_cast<SphereShape*>(shapeB);
		DirectX::BoundingSphere sphere(transformB->GetPos() + sphereShape->m_offset, sphereShape->m_radius);
		CollisionMeshResult result;
		isColliding = MeshIntersect(*meshShape->m_spModel->GetMesh(0), sphere, transformA->GetMatrix(), &result);
	}
	//球vsポリゴン
	else if (shapeA->GetType() == Shape::Type::Sphere && shapeB->GetType() == Shape::Type::Polygon)
	{
		auto* sphereShape = static_cast<SphereShape*>(shapeA);
		auto* polygonShape = static_cast<PolygonShape*>(shapeB);

		DirectX::BoundingSphere sphere(transformA->GetPos() + sphereShape->m_offset, sphereShape->m_radius);

		isColliding= CheckSpherePolygonIntersection(sphere, polygonShape, transformB->GetMatrix());
	}
	//ポリゴンvs球
	else if (shapeA->GetType() == Shape::Type::Polygon && shapeB->GetType() == Shape::Type::Sphere)
	{
		auto* sphereShape = static_cast<SphereShape*>(shapeB);
		auto* polygonShape = static_cast<PolygonShape*>(shapeA);

		DirectX::BoundingSphere sphere(transformB->GetPos() + sphereShape->m_offset, sphereShape->m_radius);

		isColliding = CheckSpherePolygonIntersection(sphere, polygonShape, transformA->GetMatrix());
	}

	if (!isColliding)
	{
		return;
	}

	//どちらかがトリガーだった場合
	if (colA->isTrigger() || colB->isTrigger())
	{
		CollisionInfo infoA, infoB;
		infoA.otherObject = ownerB->shared_from_this();
		infoB.otherObject = ownerA->shared_from_this();

		//簡易的法線
		infoA.contactNormal = transformB->GetPos() - transformA->GetPos();
		infoB.contactNormal = -infoA.contactNormal;

		//通知
		for (const auto& comp : ownerA->GetComponents())
		{
			if (auto receiver = std::dynamic_pointer_cast<ITriggerReceiver>(comp))
			{
				receiver->OnTriggerEnter(infoA);
			}
		}
		for (const auto& comp : ownerB->GetComponents())
		{
			if (auto receiver = std::dynamic_pointer_cast<ITriggerReceiver>(comp))
			{
				receiver->OnTriggerEnter(infoB);
			}
		}

		//トリガーなので押し出しはしない
		return;
	}

	//衝突が確認出来たら各処理をする
	auto rigidA = ownerA->GetComponent<RigidbodyComponent>();
	auto rigidB = ownerB->GetComponent<RigidbodyComponent>();

	//どっちも動かないなら衝突解決は不要
	if (!rigidA && !rigidB)return;

	//ボディタイプで役割を判別
	bool isADynamic = rigidA && rigidA->m_type == RigidbodyType::Dynamic;
	bool isBDynamic = rigidB && rigidB->m_type == RigidbodyType::Dynamic;
	bool isAStaticOrKinematic = !rigidA || rigidA->m_type == RigidbodyType::Kinematic;
	bool isBStaticOrKinematic = !rigidB || rigidB->m_type == RigidbodyType::Kinematic;

	//Aが動的、Bが静的,または運動学的な場合
	if (isADynamic&&!isBDynamic)
	{
		Pushout(ownerA, rigidA.get(), shapeA, ownerB, shapeB);
	}
	//Bが動的、Aが静的、静的,または運動学的な場合
	else if (!isADynamic&&isBDynamic)
	{
		Pushout(ownerB, rigidB.get(), shapeB, ownerA, shapeA);
	}
	//両方動的
	else if (isADynamic&&isBDynamic)
	{
		return;
	}

	// --- ゴール判定 ---
	bool isPlayerA = ownerA->GetName() == "Player";
	bool isPlayerB = ownerB->GetName() == "Player";
	bool isGoalA = ownerA->GetName()._Starts_with("GoalBlock");
	bool isGoalB = ownerB->GetName()._Starts_with("GoalBlock");

	if ((isPlayerA && isGoalB) || (isPlayerB && isGoalA))
	{
		// 現在のシーンからタイマーを取得
		if (auto scene = SceneManager::Instance().GetCurrentScene())
		{
			if (auto timerObj = scene->FindObject("Timer"))
			{
				if (auto timerComp = timerObj->GetComponent<TimerComponent>())
				{
					// GameManagerに最終的なタイムを記録
					GameManager::Instance().SetFinalTime(timerComp->GetElapsedTime());
				}
			}
		}

		// ResultSceneへの切り替え予約
		SceneManager::Instance().ChangeScene(SceneManager::SceneType::Result);
		return; // ゴールしたので他の処理は不要
	}
}

void PhysicsSystem::Pushout(GameObject* dynamicObj, RigidbodyComponent* rigid, Shape* dynamicShape, GameObject* staticObj, Shape* staticShape)
{
	auto dynamicTransform = rigid->GetTransform();
	auto staticTransform = staticObj->GetComponent<TransformComponent>();
	if (!dynamicTransform || !staticTransform)return;

	Math::Vector3 pushDir;//衝突した面の法線
	bool bPushOut = false;
	bool isJumpBlock = staticObj->GetName()._Starts_with("JumpBlock");

	//球 vs メッシュ
	if (dynamicShape->GetType() == Shape::Type::Sphere && staticShape->GetType() == Shape::Type::Mesh)
	{
		auto* sphereShape = static_cast<SphereShape*>(dynamicShape);
		auto* meshShape = static_cast<MeshShape*>(staticShape);

		DirectX::BoundingSphere sphere;
		sphere.Center = dynamicTransform->GetPos() + sphereShape->m_offset;
		sphere.Radius = sphereShape->m_radius;

		CollisionMeshResult result;
		if (MeshIntersect(*meshShape->m_spModel->GetMesh(0), sphere, staticTransform->GetMatrix(), &result))
		{
			bPushOut = true;
			pushDir = result.m_hitDir;
			float pushDist = result.m_overlapDistance;
			Math::Vector3 push = pushDir * pushDist;
			dynamicTransform->Move(push);
			
			auto& vel = rigid->m_velocity;

			if (isJumpBlock && pushDir.y > 0.7f)
			{
				//何もしない
			}
			else if (DirectX::XMVectorGetY(result.m_hitNDir) > 0.7f)//法線が真上に近い(地面)
			{
				if (vel.y < 0)
				{
					vel.y = 0;//バウンドさせない
				}
			}
			else//法線が横や下を向いている(壁や天井)
			{
				vel = DirectX::XMVector3Reflect(vel, result.m_hitNDir);
				vel *= 0.8f;
			}
		}
	}
	//球 vs 箱
	else if (dynamicShape->GetType() == Shape::Type::Sphere && staticShape->GetType() == Shape::Type::Box)
	{
		auto* sphereShape = static_cast<SphereShape*>(dynamicShape);
		auto* boxShape = static_cast<BoxShape*>(staticShape);

		DirectX::BoundingSphere sphere;
		sphere.Center = dynamicTransform->GetPos() + sphereShape->m_offset;
		sphere.Radius = sphereShape->m_radius;

		DirectX::BoundingOrientedBox obb;
		obb.Center = staticTransform->GetPos() + boxShape->m_offset;
		obb.Extents = boxShape->m_extents;
		
		Math::Quaternion q;
		q.CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(staticTransform->GetRot().y),
			DirectX::XMConvertToRadians(staticTransform->GetRot().x),
			DirectX::XMConvertToRadians(staticTransform->GetRot().z)
		);
		obb.Orientation = q;

		if (sphere.Intersects(obb))
		{
			bPushOut = true;
			//1:球の中心をローカル座標系に変換
			Math::Vector3 sphereCenter(sphere.Center);
			Math::Vector3 obbCenter(obb.Center);
			Math::Vector3 sphereCenterInBoxSpace = sphereCenter - obbCenter;
			sphereCenterInBoxSpace = XMVector3InverseRotate(sphereCenterInBoxSpace, q);

			//2:箱の表面上の最近接点をローカル座標系で求める
			Math::Vector3 closestPointInBoxSpace;
			closestPointInBoxSpace.x = std::clamp(sphereCenterInBoxSpace.x, -obb.Extents.x, obb.Extents.x);
			closestPointInBoxSpace.y = std::clamp(sphereCenterInBoxSpace.y, -obb.Extents.y, obb.Extents.y);
			closestPointInBoxSpace.z = std::clamp(sphereCenterInBoxSpace.z, -obb.Extents.z, obb.Extents.z);

			//3:最近接点をワールド座標系に戻す
			Math::Vector3 closestPointWorld = XMVector3Rotate(closestPointInBoxSpace, q);
			closestPointWorld += obb.Center;

			//4:押し出しベクトルと量を計算
			pushDir = Math::Vector3(sphere.Center) - closestPointWorld;
			float dist = pushDir.Length();
			float overlap = sphere.Radius - dist;

			if (overlap > 0)
			{
				pushDir.Normalize();
				Math::Vector3 pushVector = pushDir * overlap;

				//5:動的オブジェクトのみ押し出す
				dynamicTransform->Move(pushVector);

				//速度の反射//pushDirが法線の代わりになる
				/*auto& vel = rigid->m_velocity;
				vel = DirectX::XMVector3Reflect(vel, pushDir);
				vel *= 0.8f;*/
			}
		}
	}
	//球vsポリゴン
	else if (dynamicShape->GetType() == Shape::Type::Sphere && staticShape->GetType() == Shape::Type::Polygon)
	{
		auto* sphereShape = static_cast<SphereShape*>(dynamicShape);
		auto* polygonShape = static_cast<PolygonShape*>(staticShape);

		DirectX::BoundingSphere sphere;
		sphere.Center = dynamicTransform->GetPos() + sphereShape->m_offset;
		sphere.Radius = sphereShape->m_radius;

		CollisionMeshResult result;
		if (CalculatePolygonSpherePushout(polygonShape, sphere, staticTransform->GetMatrix(), &result))
		{
			bPushOut = true;
			pushDir = result.m_hitDir;
			float pushDist = result.m_overlapDistance;

			dynamicTransform->Move(pushDir* pushDist);

			auto& vel = rigid->m_velocity;
			if (rigid->m_isOnSlipperySurface)
			{
				// 滑っているときは速度を一切いじらない
			}
			else if (isJumpBlock && pushDir.y > 0.7f)
			{
				//何もしない
			}
			else if (DirectX::XMVectorGetY(result.m_hitNDir) > 0.7f)
			{
				if (vel.y < 0) { vel.y = 0; }
			}
			else
			{
				vel = DirectX::XMVector3Reflect(vel, result.m_hitNDir);
				vel *= 0.8f;
			}
		}
	}

	//--通知処理--
	if (bPushOut)
	{
		CollisionInfo dynamicInfo, staticInfo;
		dynamicInfo.otherObject = staticObj->shared_from_this();
		dynamicInfo.contactNormal = pushDir;

		staticInfo.otherObject = dynamicObj->shared_from_this();
		staticInfo.contactNormal = pushDir;

		//動的オブジェクトに通知
		for (const auto& comp : dynamicObj->GetComponents()) {
			if (auto receiver = std::dynamic_pointer_cast<ICollisionReceiver>(comp)) {
				receiver->OnCollision(dynamicInfo);
			}
		}

		//静的オブジェクトに通知
		for (const auto& comp : staticObj->GetComponents()) {
			if (auto receiver = std::dynamic_pointer_cast<ICollisionReceiver>(comp)) {
				receiver->OnCollision(staticInfo);
			}
		}
	}
}

bool PhysicsSystem::CheckSpherePolygonIntersection(const DirectX::BoundingSphere& spahe, PolygonShape* polygon, const Math::Matrix& mat)
{
	//頂点リスト取得
	const auto& positions = polygon->GetVertices();
	const auto& faces = polygon->GetFaces();

	//球情報をローカル座標系に変換
	DirectX::XMVECTOR spherePosInv = {};
	DirectX::XMVECTOR objScale = {};
	float radiusSqr = 0.0f;
	InvertSphereInfo(spherePosInv, objScale, radiusSqr, mat, spahe);

	//全ての面と判定
	for (const KdMeshFace& face : faces)
	{
		DirectX::XMVECTOR nearPos;

		const Math::Vector3& v0 = positions[face.Idx[0]];
		const Math::Vector3& v1 = positions[face.Idx[1]];
		const Math::Vector3& v2 = positions[face.Idx[2]];

		//点と三角形の最近接点を計算
		KdPointToTriangle(spherePosInv, v0, v1, v2, nearPos);

		DirectX::XMVECTOR vecToCenter = DirectX::XMVectorSubtract(spherePosInv, nearPos);

		//拡縮を考慮した距離の2乗で比較
		vecToCenter = DirectX::XMVectorMultiply(vecToCenter, objScale);

		//.m128_f32[0]はXMVECTOR変数を4つのfloat値が入った配列として扱い、[0]番目の値を取り出している
		if (DirectX::XMVector3LengthSq(vecToCenter).m128_f32[0] <= radiusSqr)
		{
			return true;
		}
	}
	return false;
}

bool PhysicsSystem::CalculateSpherePolygonPushout(const DirectX::BoundingSphere& shape, PolygonShape* polygon, const Math::Matrix& mat, CollisionMeshResult* result)
{
	bool isHit = false;
	float deepestPenetration = -1.0f;//一番深い距離を保持
	CollisionMeshResult finalResult;//そのリザルトを保持

	//頂点リスト取得
	std::vector<Math::Vector3> positions;
	positions = polygon->GetVertices();
	const auto& faces = polygon->GetFaces();

	//球情報をローカル座標系に変換
	DirectX::XMVECTOR finalPos = {};
	DirectX::XMVECTOR objScale = {};
	float radiusSqr = 0.0f;
	InvertSphereInfo(finalPos, objScale, radiusSqr, mat, shape);

	//全ての面と判定
	for (const KdMeshFace& face:faces)
	{
		DirectX::XMVECTOR nearPos;

		const Math::Vector3& v0 = positions[face.Idx[0]];
		const Math::Vector3& v1 = positions[face.Idx[1]];
		const Math::Vector3& v2 = positions[face.Idx[2]];

		//点と三角形の最近接点を計算
		KdPointToTriangle(finalPos, v0, v1, v2, nearPos);

		//最近接点から球の中心座標へのベクトル計算
		DirectX::XMVECTOR vecToCenter = DirectX::XMVectorSubtract(finalPos, nearPos);

		//拡縮を考慮した距離(スケール空間に変換)
		DirectX::XMVECTOR scaledVecToCenter = DirectX::XMVectorMultiply(vecToCenter, objScale);

		//めり込んでなかったらスキップ
		if (DirectX::XMVector3LengthSq(scaledVecToCenter).m128_f32[0] > radiusSqr)
		{
			continue;
		}

		//押し返すためのベクトルを計算
		DirectX::XMVECTOR vPush = DirectX::XMVector3Normalize(scaledVecToCenter);//方向ベクトル決定
		vPush = DirectX::XMVectorScale(vPush, shape.Radius - DirectX::XMVector3Length(scaledVecToCenter).m128_f32[0]);//めり込み量でスケーリング
		vPush = DirectX::XMVectorDivide(vPush, objScale);//拡縮を考慮した座標系に戻す

		//floatのめり込み量を取得
		float currentPenetration = DirectX::XMVector3Length(vPush).m128_f32[0];

		//一番深いめり込み量か(誤差を無視するために0より大きいかも考慮)
		if (currentPenetration > 0.0001f && currentPenetration > deepestPenetration)
		{
			isHit = true;
			deepestPenetration = currentPenetration;

			//現時点での押し出し情報を保存
			finalResult.m_hit = true;
			finalResult.m_overlapDistance = currentPenetration;
			finalResult.m_hitDir = DirectX::XMVector3Normalize(vPush);
		}
	}

	if (isHit)
	{
		*result = finalResult;
	}
	return isHit;
}
