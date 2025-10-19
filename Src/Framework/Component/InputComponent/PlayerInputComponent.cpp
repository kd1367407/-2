#include "PlayerInputComponent.h"
#include"../GameObject.h"
#include"../RigidbodyComponent/RigidbodyComponent.h"
#include"../../Command/CommandInvoker/CommandInvoker.h"
#include"../../Command/MoveCommand/MoveCommand.h"
#include"../GravityComponent/GravityComponent.h"
#include"../Src/Application/main.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../Src/Application/Scene/BaseScene/BaseScene.h"
#include"../CameraComponent/TPSCameraComponent/TPSCameraComponent.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/SettingsManager/SettingsManager.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void PlayerInputComponent::Awake()
{
	const auto& settings = SettingsManager::Instance().GetGameSetting()["player_settings"];
	m_maxSpeed = JsonHelper::GetFloat(settings, "max_speed", 5.0f);
	m_moveForce = JsonHelper::GetFloat(settings, "move_force", 15.0f);
	m_controlForce = JsonHelper::GetFloat(settings, "control_force", 15.0f);
	m_brakeForce = JsonHelper::GetFloat(settings, "brake_force", 20.0f);
	m_jumpPower = JsonHelper::GetFloat(settings, "jump_power", 800.0f);
	m_coyoteTimeDuration = JsonHelper::GetFloat(settings, "coyote_time", 0.1f);
}

void PlayerInputComponent::Start()
{
	//自身がアタッチされているGameObjectから必要なコンポーネントを取得
	m_rigid = m_owner->GetComponent<RigidbodyComponent>();
	m_gravity = m_owner->GetComponent<GravityComponent>();
}

void PlayerInputComponent::Update()
{
	if (!m_invoker || !m_rigid || !m_gravity)return;

	if (SceneManager::Instance().GetCurrentMode() != SceneManager::SceneMode::Game)return;

	bool isOnGround = m_gravity->IsOnGround();
	float verticalVelocity = m_rigid->m_velocity.y;

	//--コヨーテタイムの更新--
	if (isOnGround&&verticalVelocity<=0.0f)
	{
		//地面判定の間はカウンターリセット
		m_coyoteTimeCounter = m_coyoteTimeDuration;
	}
	else
	{
		//地面判定でなければカウンターを減らす
		if (m_coyoteTimeCounter > 0.0f)
		{
			float deltatime = Application::Instance().GetDeltaTime();
			m_coyoteTimeCounter -= deltatime;
		}
	}

	//カウンターが残っている時にジャンプ可能
	bool canJump = m_coyoteTimeCounter > 0.0f;

	//ジャンプ
	if (KdInputManager::Instance().IsPress("Jump"))
	{
		if (canJump)
		{
			m_rigid->m_velocity.y = 0.0f;
			float jumpPower = 800.0f;
			auto command = std::make_unique<MoveCommand>(m_rigid, Math::Vector3(0, jumpPower, 0));
			m_invoker->ExecuteCommand(std::move(command));
			
			//ジャンプしたら即座にコヨーテタイムを消費
			m_coyoteTimeCounter = 0.0f;
		}
	}

	//水平方向の移動
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

	Math::Vector3 currentVelocity = m_rigid->m_velocity;
	currentVelocity.y = 0;
	
	if (moveDir.LengthSquared() > 0)
	{
		moveDir.Normalize();

		if (auto spCam = m_wpCamera.lock())
		{
			//最新のyawを取得
			Math::Matrix camRotY = Math::Matrix::CreateRotationY(spCam->GetYaw());

			//カメラの回転に合わせて進む方向を回転
			moveDir = Math::Vector3::TransformNormal(moveDir, camRotY);
		}

		float moveForce = 15.0f;

		//目標速度に近づける力を計算
		float maxSpeed = 5.0f;
		Math::Vector3 targetVelocity = moveDir * maxSpeed;
		Math::Vector3 velocityDiff = targetVelocity - currentVelocity;

		float controlForce = 15.0f;
		if (m_rigid->m_isOnSlipperySurface)
		{
			controlForce = 10.0f;
		}

		//移動コマンドを生成して実行を依頼
		auto command = std::make_unique<MoveCommand>(m_rigid, velocityDiff * controlForce);
		m_invoker->ExecuteCommand(std::move(command));
	}
	else if (isOnGround && !m_rigid->m_isOnSlipperySurface)
	{
		float brakeForce = 20.0f;
		auto command = std::make_unique<MoveCommand>(m_rigid, -currentVelocity * brakeForce);
		m_invoker->ExecuteCommand(std::move(command));
	}
}

void PlayerInputComponent::SetInvoker(std::shared_ptr<CommandInvoker> invoker)
{
	m_invoker = invoker;
}
