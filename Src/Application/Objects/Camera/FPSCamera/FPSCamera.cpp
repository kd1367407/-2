#include "FPSCamera.h"

void FPSCamera::Init()
{
	// 親クラスの初期化呼び出し
	CameraBase::Init();

	// 基準点(ターゲット)の目線
	m_mLocalPos = Math::Matrix::CreateTranslation(0, 1.5f, 0.0f);

	SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);
}

void FPSCamera::PostUpdate()
{
	// ターゲットの行列(有効な場合利用する)
	Math::Matrix								_targetMat = Math::Matrix::Identity;
	const std::shared_ptr<const KdGameObject>	_spTarget = m_wpTarget.lock();
	if (_spTarget)
	{
		_targetMat = Math::Matrix::CreateTranslation(_spTarget->GetPos());
	}

	// カメラの回転
	if (!(GetAsyncKeyState(VK_LSHIFT) & 0x8000))
	{
		UpdateRotateByMouse();
	}
	m_mRot = GetRotationMatrix();
	m_mWorld = m_mRot * m_mLocalPos * _targetMat;
}
