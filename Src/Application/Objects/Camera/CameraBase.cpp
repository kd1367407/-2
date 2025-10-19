#include "CameraBase.h"
#include"../../Scene/SceneManager.h"

void CameraBase::Init()
{
	//実体化
	m_spCamera = std::make_shared<KdCamera>();

	//マウスカーソル中央固定
	m_fixMousePos = { 640,360 };
}

void CameraBase::PreDraw()
{
	if (!m_spCamera)return;

	m_spCamera->SetCameraMatrix(m_mWorld);
}

void CameraBase::OnImGui()
{
	KdGameObject::OnImGui();

	ImGui::Spacing();
	ImGui::Text("CameraParameters");
	ImGui::DragFloat3("Angle", &m_degAng.x, 0.1f);

	//ターゲット情報表示
	if (auto target = m_wpTarget.lock())
	{
		ImGui::Text("Target: %s (%p)", target->GetTypeName(), target.get());
	}
	else
	{
		ImGui::Text("Target: None");
	}
}

void CameraBase::SetTarget(const std::shared_ptr<KdGameObject>& target)
{
	if (target)
	{
		m_wpTarget = target;
	}
}

const Math::Matrix CameraBase::GetRotationMatrix() const
{
	return Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_degAng.y),
		DirectX::XMConvertToRadians(m_degAng.x),
		DirectX::XMConvertToRadians(m_degAng.z)
	);
}

const Math::Matrix CameraBase::GetRotationYMatrix() const
{
	return Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_degAng.y));
}

void CameraBase::RegistHitObject(const std::shared_ptr<KdGameObject>& object)
{
	if (object)
	{
		m_wpHitObjList.push_back(object);
	}
}

void CameraBase::UpdateRotateByMouse()
{
	//ゲームモードの時のみ有効
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Game)
	{
		POINT nowPos;
		GetCursorPos(&nowPos);

		POINT mouseMove;
		mouseMove.x = nowPos.x - m_fixMousePos.x;
		mouseMove.y = nowPos.y - m_fixMousePos.y;

		SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);

		m_degAng.x += mouseMove.y * 0.15f;
		m_degAng.y += mouseMove.x * 0.15f;

		//回転角度の制限
		m_degAng.x = std::clamp(m_degAng.x, -80.0f, 80.0f);
	}
}
