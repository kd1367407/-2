#pragma once
#include <DirectXMath.h>

void InvertSphereInfo(DirectX::XMVECTOR& spherePosInv, DirectX::XMVECTOR& sphereScale, float& radiusSqr,
	const DirectX::XMMATRIX& matrix, const DirectX::BoundingSphere& sphere);

bool HitCheckAndPosUpdate(DirectX::XMVECTOR& finalPos, DirectX::XMVECTOR& finalHitPos, std::vector<Math::Vector3>& finalFace,
	const std::vector<Math::Vector3>& nearFace, const DirectX::XMVECTOR& nearPoint, const DirectX::XMVECTOR& objScale, float radiusSqr, float sphereRadius);

void SetSphereResult(CollisionMeshResult& result, bool isHit, const DirectX::XMVECTOR& hitPos,
	const DirectX::XMVECTOR& finalPos, std::vector<Math::Vector3>& finalFace, const DirectX::XMVECTOR& beginPos);

void KdPointToTriangle(DirectX::FXMVECTOR p, DirectX::FXMVECTOR a, DirectX::FXMVECTOR b, DirectX::FXMVECTOR c, DirectX::XMVECTOR& outPt);

// 反復的な押し出し計算の結果を格納する
void SetIterativePushoutResult(
	CollisionMeshResult& result,
	bool isHit,
	const DirectX::XMVECTOR& initialSpherePos_local, // 計算前の球の中心（ローカル座標）
	const DirectX::XMVECTOR& finalSpherePos_local,   // 押し出された後の球の中心（ローカル座標）
	const DirectX::XMMATRIX& matrix                  // ワールド行列
);
