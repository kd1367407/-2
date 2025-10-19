#include "EditorCameraComponent.h"
#include"../../GameObject.h"
#include"../../TransformComponent/TransformComponent.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../Src/Application/main.h"
#include"../Src/Application/SettingsManager/SettingsManager.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../Src/Application/Scene/BaseScene/BaseScene.h"

void EditorCameraComponent::Awake()
{
	m_spCamera = std::make_shared<KdCamera>();
	GetCursorPos(&m_preMousePos);
	m_spCamera->SetProjectionMatrix(60.0f);

	//設定ファイルからカメラパラメータを読み込み
	const auto& settings = SettingsManager::Instance().GetGameSetting();
	const auto& camSettings = settings["camera_settings"];
	m_sensitivity = JsonHelper::GetFloat(camSettings, "editor_sensitivity", 0.4f);
	m_moveSpeed = JsonHelper::GetFloat(camSettings, "editor_move_speed", 10.0f);
}

void EditorCameraComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();
}

void EditorCameraComponent::Update()
{
	auto scene = SceneManager::Instance().GetCurrentScene();
	//自身がアクティブカメラでなければスキップ
	if (!scene || scene->GetActiveCamera() != shared_from_this())return;

	if (SceneManager::Instance().GetCurrentMode() != SceneManager::SceneMode::Create)return;
	if (!m_transform)return;

	POINT nowPos{};
	GetCursorPos(&nowPos);

	//マウスによる回転制御
	if (KdInputManager::Instance().IsHold("CameraRotate"))
	{
		float deltaX = static_cast<float>(nowPos.x - m_preMousePos.x);
		float deltaY = static_cast<float>(nowPos.y - m_preMousePos.y);

		m_yaw += DirectX::XMConvertToRadians(deltaX * m_sensitivity);
		m_pitch += DirectX::XMConvertToRadians(deltaY * m_sensitivity);
		m_pitch = std::clamp(m_pitch, DirectX::XMConvertToRadians(-89.0f), DirectX::XMConvertToRadians(89.0f));
	}

	m_preMousePos = nowPos;

	//キーボードによる操作
	Math::Vector3 moveDir = Math::Vector3::Zero;
	if (KdInputManager::Instance().IsHold("MoveForward"))
	{
		moveDir.z = 1.0f;
	}
	if (KdInputManager::Instance().IsHold("MoveBack"))
	{
		moveDir.z = -1.0f;
	}
	if (KdInputManager::Instance().IsHold("MoveLeft"))
	{
		moveDir.x = -1.0f;
	}
	if (KdInputManager::Instance().IsHold("MoveRight"))
	{
		moveDir.x = 1.0f;
	}

	if (moveDir.LengthSquared() > 0.0f)
	{
		//カメラの方向に合わせて移動方向を修正
		Math::Matrix camRotMat = Math::Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.0f);
		moveDir = Math::Vector3::TransformNormal(moveDir, camRotMat);

		float deltaTime = Application::Instance().GetDeltaTime();
		m_transform->Move(moveDir * m_moveSpeed * deltaTime);
	}

	Math::Vector3 newRot;
	newRot.x = DirectX::XMConvertToDegrees(m_pitch);
	newRot.y = DirectX::XMConvertToDegrees(m_yaw);
	newRot.z = 0.0f;
	m_transform->SetRot(newRot);

	Math::Matrix finalCamMat = Math::Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.0f) * Math::Matrix::CreateTranslation(m_transform->GetPos());

	//TransformComponentとKdCameraに計算した最新のカメラ行列を直接設定
	m_transform->SetMatrix(finalCamMat);
	m_spCamera->SetCameraMatrix(finalCamMat);
}


