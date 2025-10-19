#include "TPSCameraComponent.h"
#include"../Src/Application/SettingsManager/SettingsManager.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../Src/Application/main.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../../TransformComponent/TransformComponent.h"
#include"../Src/Application/System/PhysicsSystem.h"

void TPSCameraComponent::Awake()
{
	CameraComponent::Awake();

	// 設定ファイルからカメラパラメータを読み込み
	const auto& settings = SettingsManager::Instance().GetGameSetting();
	const auto& camSettings = settings["camera_settings"];
	m_sensitivity = JsonHelper::GetFloat(camSettings, "tps_sensitivity", 0.2f);
	float height = JsonHelper::GetFloat(camSettings, "tps_height", 3.0f);
	float distance = JsonHelper::GetFloat(camSettings, "tps_distance", -7.0f);

	//ターゲットからの相対的な初期座標
	m_localMat = Math::Matrix::CreateTranslation(0.0f, height, distance);

	//画面の中心を計算して保存
	RECT clientRect;
	GetClientRect(Application::Instance().GetWindowHandle(), &clientRect);
	m_fixMousePos.x = clientRect.right / 2;
	m_fixMousePos.y = clientRect.bottom / 2;
	//アプリケーションのクライアント座標をスクリーン座標に変換
	ClientToScreen(Application::Instance().GetWindowHandle(), &m_fixMousePos);
}

void TPSCameraComponent::Update()
{
	//ゲームモードではない、またはターゲットがなければ何もしない
	if (SceneManager::Instance().GetCurrentMode() != SceneManager::SceneMode::Game || m_wpTarget.expired())return;

	//マウスの現在の座標を取得
	POINT nowMousePos;
	GetCursorPos(&nowMousePos);

	//前フレームからのマウスの移動量を計算
	float deltaX = static_cast<float>(nowMousePos.x - m_fixMousePos.x);
	float deltaY = static_cast<float>(nowMousePos.y - m_fixMousePos.y);

	//カーソルを画面中央に戻す
	SetCursorPos(m_fixMousePos.x, m_fixMousePos.y);

	//移動量をカメラの回転角度に変換
	const float sensitivity = 0.2f;
	m_yaw += DirectX::XMConvertToRadians(deltaX * sensitivity);
	m_pitch += DirectX::XMConvertToRadians(deltaY * sensitivity);

	//pitchに制限
	m_pitch = std::clamp(m_pitch, DirectX::XMConvertToRadians(-80.0f), DirectX::XMConvertToRadians(80.0f));
}

void TPSCameraComponent::PostUpdate()
{
	if (!m_transform) return;
	auto spTarget = m_wpTarget.lock();

	//ターゲットが存在し、かつ自分自身ではないことを確認
	if (!spTarget || spTarget.get() == m_transform.get()) return;

	Math::Matrix targetMat = Math::Matrix::CreateTranslation(spTarget->GetPos());
	Math::Matrix rotY = Math::Matrix::CreateRotationY(m_yaw);
	Math::Matrix rotX = Math::Matrix::CreateRotationX(m_pitch);
	Math::Matrix finalMat = m_localMat * rotX * rotY * targetMat;

	//カメラがオブジェクトと衝突することを考慮
	RayInfo ray;
	ray.m_start = targetMat.Translation() + Math::Vector3(0.0f, 1.0f, 0.0f);
	ray.m_dir = finalMat.Translation() - ray.m_start;

	float rayDistance = ray.m_dir.Length();
	if (rayDistance > 0.0f)
	{
		ray.m_maxDistance = rayDistance;
		ray.m_dir.Normalize();

		RayResult result;
		if (PhysicsSystem::Instance().Raycast(ray, result, LayerAll, spTarget->GetOwner()))
		{
			finalMat.Translation(result.m_hitPos - ray.m_dir * 0.1f);
		}
	}

	//計算した最終行列から座標と回転を分解
	Math::Vector3 finalPos, dummyScale, finalRot;
	Math::Quaternion dummyQuat;
	finalMat.Decompose(dummyScale, dummyQuat, finalPos);

	finalRot = {
		DirectX::XMConvertToDegrees(m_pitch),
		DirectX::XMConvertToDegrees(m_yaw),
		0.0f
	};

	m_transform->SetPos(finalPos);
	m_transform->SetRot(finalRot);

	m_spCamera->SetCameraMatrix(m_transform->GetMatrix());

}

void TPSCameraComponent::SetTarget(const std::shared_ptr<TransformComponent>& target)
{
	if (target)
	{
		m_wpTarget = target;
	}
}
