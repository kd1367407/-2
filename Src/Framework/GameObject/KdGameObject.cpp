#include "KdGameObject.h"

void KdGameObject::PostUpdate()
{
	if (m_isDirty)
	{
		//拡縮行列計算
		Math::Matrix scaleMat = Math::Matrix::CreateScale(m_scale);
		
		//回転行列計算
		Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(m_rot.y),
			DirectX::XMConvertToRadians(m_rot.x),
			DirectX::XMConvertToRadians(m_rot.z)
		);

		//座標行列計算
		Math::Matrix transMat = Math::Matrix::CreateTranslation(m_pos);

		//行列合成
		m_mWorld = scaleMat * rotMat * transMat;

		m_isDirty = false;
	}
}

void KdGameObject::DrawDebug()
{
	// 早期リターン
	if (!m_pDebugWire)return;

	m_pDebugWire->Draw();
}

void KdGameObject::CalcDistSqrFromCamera(const Math::Vector3& camPos)
{
	m_distSqrFromCamera = (m_mWorld.Translation() - camPos).LengthSquared();
}

bool KdGameObject::Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetBox, m_mWorld, pResults);
}

bool KdGameObject::Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults)
{
	if (!m_pCollider) { return false; }

	return m_pCollider->Intersects(targetShape, m_mWorld, pResults);
}
