#include "KdCollision.h"
#include"../Component/ColliderComponent/CollisionUtils/CollisionUtils.h"
using namespace DirectX;

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// レイの当たり判定
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイの情報を逆行列化する
// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
// レイとポリゴンを判定する際に全ての頂点を行列移動させるとポリゴン数によって処理コストが変わるため非常に不安定
// レイの情報は1つしかないためレイだけを逆行列化する事で処理の安定化＋1度しか計算が行われないため最大の効率化にもなる
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void InvertRayInfo(DirectX::XMVECTOR& rayPosInv, DirectX::XMVECTOR& rayDirInv, float& rayRangeInv, float& scaleInv,
	const DirectX::XMMATRIX& matrix, const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir, float rayRange)
{
	// ターゲットの逆行列でレイを変換
	DirectX::XMMATRIX invMat = XMMatrixInverse(0, matrix);

	// レイの判定開始位置を逆変換
	rayPosInv = XMVector3TransformCoord(rayPos, invMat);

	// レイの方向を逆変換
	rayDirInv = XMVector3TransformNormal(rayDir, invMat);

	// 拡大率を逆変換
	scaleInv = DirectX::XMVector3Length(rayDirInv).m128_f32[0];

	// レイの方向ベクトルの長さ=拡大率で判定限界距離を補正
	// ※逆行列に拡縮が入っていると、レイの長さが変わるため
	// レイが当たった座標からの距離に拡縮が反映されてしまうので
	// 判定用の最大距離にも拡縮を反映させておく
	rayRangeInv = rayRange * scaleInv;

	// 方角レイとして正しく扱うためには長さが１でなければならないので正規化
	rayDirInv = DirectX::XMVector3Normalize(rayDirInv);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイとの当たり判定結果をリザルトにセットする
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
static void SetRayResult(CollisionMeshResult& result, bool isHit, float closestDist,
	const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir, float rayRange, std::vector<Math::Vector3>& finalFace)
{
	// リザルトに結果を格納
	result.m_hit = isHit;

	result.m_hitPos = DirectX::XMVectorAdd(rayPos, DirectX::XMVectorScale(rayDir, closestDist));

	result.m_hitDir = DirectX::XMVectorScale(rayDir, -1);

	result.m_overlapDistance = rayRange - closestDist;

	// HITした面の法線を計算する
	Math::Vector3 _normalV1 = finalFace[1] - finalFace[0];
	Math::Vector3 _normalV2 = finalFace[2] - finalFace[0];

	result.m_hitNDir = _normalV1.Cross(_normalV2);
	result.m_hitNDir = DirectX::XMVector3Normalize(result.m_hitNDir);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイ対ポリゴン(KdMesh以外の任意の多角形ポリゴン)の当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool PolygonsIntersect(const KdPolygon& poly, const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir, float rayRange,
	const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//--------------------------------------------------------
	// レイの逆行列化
	//--------------------------------------------------------
	DirectX::XMVECTOR rayPosInv, rayDirInv;
	float rayRangeInv = 0;
	float scaleInv = 0;

	InvertRayInfo(rayPosInv, rayDirInv, rayRangeInv, scaleInv,
		matrix, rayPos, rayDir, rayRange);

	//--------------------------------------------------------
	// レイ vs 全ての面
	//--------------------------------------------------------

	// ヒット判定
	bool isHit = false;
	float closestDist = FLT_MAX;
	float closestPrevDist = -FLT_MAX;
	std::vector<Math::Vector3>	finalFace = {};	// 当たった面の中でも最後の面

	// 頂点リスト取得
	std::vector<Math::Vector3> positions;
	poly.GetPositions(positions);
	size_t faceNum = positions.size() - 2;

	// 全ての面(三角形)
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// レイと三角形の判定
		float	hitDist = FLT_MAX;
		bool	_isHit = false;
		_isHit = DirectX::TriangleTests::Intersects(rayPosInv, rayDirInv,
			positions[faceIdx], positions[faceIdx + 1], positions[faceIdx + 2],
			hitDist);
		if (!_isHit) continue;

		// レイの判定範囲外なら無視
		if (hitDist > rayRangeInv) { continue; }

		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult) { return _isHit; }

		// 最短距離の更新判定処理
		closestDist = std::min(hitDist, closestDist);

		// 最短距離が更新されていれば面情報も更新する
		if (closestPrevDist != closestDist)
		{
			finalFace = { positions[faceIdx], positions[faceIdx + 1], positions[faceIdx + 2] };
			closestPrevDist = closestDist;
		}

		isHit = _isHit;
	}

	if (pResult && isHit)
	{
		SetRayResult(*pResult, isHit, closestDist / scaleInv, rayPos, rayDir, rayRange, finalFace);
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// レイ対メッシュの当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool MeshIntersect(const KdMesh& mesh, const DirectX::XMVECTOR& rayPos, const DirectX::XMVECTOR& rayDir,
	float rayRange, const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//--------------------------------------------------------
	// ブロードフェイズ
	// 　比較的軽量なAABB vs レイな判定で、
	// 　あきらかにヒットしない場合は、これ以降の判定を中止
	//--------------------------------------------------------
	{
		// AABB vs レイ
		float AABBdist = FLT_MAX;
		DirectX::BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		if (aabb.Intersects(rayPos, rayDir, AABBdist) == false) { return false; }

		// 最大距離外なら範囲外なので中止
		if (AABBdist > rayRange) { return false; }
	}

	//--------------------------------------------------------
	// レイの逆行列化
	//--------------------------------------------------------
	DirectX::XMVECTOR rayPosInv, rayDirInv;
	float rayRangeInv = 0;
	float scaleInv = 0;

	InvertRayInfo(rayPosInv, rayDirInv, rayRangeInv, scaleInv,
		matrix, rayPos, rayDir, rayRange);

	//--------------------------------------------------------
	// ナローフェイズ
	// 　レイ vs 全ての面
	//--------------------------------------------------------

	// ヒット判定
	bool isHit = false;
	float closestDist = FLT_MAX;
	float closestPrevDist = -FLT_MAX;
	std::vector<Math::Vector3>	finalFace = {};	// 当たった面の中でも最後の面

	// DEBUGビルドでも速度を維持するため、別変数に拾っておく
	const KdMeshFace* pFaces = &mesh.GetFaces()[0];
	auto& vertices = mesh.GetVertexPositions();
	size_t faceNum = mesh.GetFaces().size();

	// 全ての面(三角形)
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// 三角形を構成する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		// レイと三角形の判定
		float	hitDist = FLT_MAX;
		bool	_isHit = false;
		_isHit = DirectX::TriangleTests::Intersects(rayPosInv, rayDirInv,
			vertices[idx[0]], vertices[idx[1]], vertices[idx[2]],
			hitDist);
		if (!_isHit) continue;

		// レイの判定範囲外なら無視
		if (hitDist > rayRangeInv) { continue; }

		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult) { return _isHit; }

		// 最短距離の更新判定処理
		closestDist = std::min(hitDist, closestDist);

		// 最短距離が更新されていれば面情報も更新する
		if (closestPrevDist != closestDist)
		{
			finalFace = { vertices[idx[0]], vertices[idx[1]], vertices[idx[2]] };
			closestPrevDist = closestDist;
		}
		isHit = _isHit;
	}

	if (pResult && isHit)
	{
		SetRayResult(*pResult, isHit, closestDist / scaleInv, rayPos, rayDir, rayRange, finalFace);
	}

	return isHit;
}


// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// スフィアの当たり判定
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// スフィア対ポリゴン(KdMesh以外の任意の多角形ポリゴン)の当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool PolygonsIntersect(const KdPolygon& poly, const DirectX::BoundingSphere& sphere, const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//------------------------------------------
	// 球とポリゴンとの詳細判定
	//------------------------------------------
	// １つでもヒットしたらtrue
	bool isHit = false;

	// 頂点リスト取得
	std::vector<Math::Vector3> positions;
	poly.GetPositions(positions);
	size_t faceNum = positions.size() - 2;

	DirectX::XMVECTOR			finalHitPos = {};	// 当たった座標の中でも最後の座標
	DirectX::XMVECTOR			finalPos = {};	// 各面に押されて最終的に到達する座標：判定する球の中心
	std::vector<Math::Vector3>	finalFace = {};	// 当たった面の中でも最後の面
	DirectX::XMVECTOR			objScale = {};	// ターゲットオブジェクトの各軸の拡大率
	float radiusSqr = 0.0f;
	InvertSphereInfo(finalPos, objScale, radiusSqr, matrix, sphere);

	// 全ての面と判定
	// ※判定はポリゴンのローカル空間で行われる
	for (UINT faceIndx = 0; faceIndx < faceNum; faceIndx++)
	{
		DirectX::XMVECTOR nearPoint;

		// 点 と 三角形 の最近接点を求める
		KdPointToTriangle(finalPos,
			positions[faceIndx],
			positions[faceIndx + 1],
			positions[faceIndx + 2],
			nearPoint);

		// 当たっているかどうかの判定と最終座標の更新
		isHit |= HitCheckAndPosUpdate(finalPos, finalHitPos, finalFace, positions, nearPoint, objScale, radiusSqr, sphere.Radius);

		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult && isHit) { return isHit; }
	}

	// リザルトに結果を格納
	if (pResult && isHit)
	{
		SetSphereResult(*pResult, isHit, XMVector3TransformCoord(finalHitPos, matrix),
			XMVector3TransformCoord(finalPos, matrix), finalFace, XMLoadFloat3(&sphere.Center));
	}

	return isHit;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// スフィア対メッシュの当たり判定本体
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool MeshIntersect(const KdMesh& mesh, const DirectX::BoundingSphere& sphere,
	const DirectX::XMMATRIX& matrix, CollisionMeshResult* pResult)
{
	//------------------------------------------
	// ブロードフェイズ
	// 　高速化のため、まずは境界ボックス(AABB)で判定
	// 　この段階でヒットしていないなら、詳細な判定をする必要なし
	//------------------------------------------
	{
		// メッシュのAABBを元に、行列で変換したAABBを作成
		DirectX::BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		if (aabb.Intersects(sphere) == false) { return false; }
	}

	//------------------------------------------
	// ナローフェイズ
	// 　球とメッシュとの詳細判定
	//------------------------------------------

	// １つでもヒットしたらtrue
	bool isHit = false;

	// DEBUGビルドでも速度を維持するため、別変数に拾っておく
	const auto* pFaces = &mesh.GetFaces()[0];
	size_t faceNum = mesh.GetFaces().size();
	auto& vertices = mesh.GetVertexPositions();

	DirectX::XMVECTOR			finalHitPos = {};	// 当たった座標の中でも最後の座標
	DirectX::XMVECTOR			finalPos = {};	// 各面に押されて最終的に到達する座標：判定する球の中心
	std::vector<Math::Vector3>	finalFace = {};	// 当たった面の中でも最後の面
	DirectX::XMVECTOR			objScale = {};	// ターゲットオブジェクトの各軸の拡大率
	float radiusSqr = 0.0f;
	InvertSphereInfo(finalPos, objScale, radiusSqr, matrix, sphere);

	// 全ての面と判定
	// ※判定はメッシュのローカル空間で行われる
	for (UINT faceIdx = 0; faceIdx < faceNum; faceIdx++)
	{
		DirectX::XMVECTOR nearPoint;

		// 三角形を構成する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].Idx;

		// 点 と 三角形 の最近接点を求める
		KdPointToTriangle(finalPos, vertices[idx[0]], vertices[idx[1]], vertices[idx[2]], nearPoint);

		// 当たっているかどうかの判定と最終座標の更新
		isHit |= HitCheckAndPosUpdate(finalPos, finalHitPos, finalFace, vertices, nearPoint, objScale, radiusSqr, sphere.Radius);

		// CollisionResult無しなら結果は関係ないので当たった時点で返る
		if (!pResult && isHit) { return isHit; }
	}

	// リザルトに結果を格納
	if (pResult && isHit)
	{
		SetSphereResult(*pResult, isHit, XMVector3TransformCoord(finalHitPos, matrix),
			XMVector3TransformCoord(finalPos, matrix), finalFace, XMLoadFloat3(&sphere.Center));
	}

	return isHit;
}