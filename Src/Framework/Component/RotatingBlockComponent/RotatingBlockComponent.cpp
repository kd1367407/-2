#include "RotatingBlockComponent.h"
#include"../Src/Application/main.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../GameObject.h"
#include"../Src/Application/GameViewModel.h"
#include"../IdComponent/IdComponent.h"

void RotatingBlockComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("RotatingBlockComponent"))return;

	const auto& rotationData = data.at("RotatingBlockComponent");

	JsonHelper::GetVector3(rotationData, "rotation_axis", m_rotationAxis, m_rotationAxis);
	m_rotationAmount = JsonHelper::GetFloat(rotationData, "rotation_amount", m_rotationAmount);
	m_rotationSpeed = JsonHelper::GetFloat(rotationData, "rotation_speed", m_rotationSpeed);
}

void RotatingBlockComponent::Awake()
{
	//何もしない。宣言することが大事
}

void RotatingBlockComponent::Start()
{
	m_wpTransform = m_owner->GetComponent<TransformComponent>();

	if (auto transform = m_wpTransform.lock())
	{
		Math::Vector3 initialEuler = transform->GetRot();

		//オイラー角をクォータニオンに変換して目標回転として初期化
		m_targetRotation.CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(initialEuler.y),
			DirectX::XMConvertToRadians(initialEuler.x),
			DirectX::XMConvertToRadians(initialEuler.z)
		);
	}
}

void RotatingBlockComponent::Update()
{
	if (m_cooldownTimer > 0.0f)
	{
		m_cooldownTimer -= Application::Instance().GetDeltaTime();
	}

	if (!m_isRotating)return;

	if (auto transform = m_wpTransform.lock())
	{
		//現在の回転取得
		Math::Vector3 currentEuler = transform->GetRot();
		Math::Quaternion currentQuat;
		currentQuat = currentQuat.CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(currentEuler.y),
			DirectX::XMConvertToRadians(currentEuler.x),
			DirectX::XMConvertToRadians(currentEuler.z)
		);

		//球面線形補完
		float deltaTime = Application::Instance().GetDeltaTime();
		Math::Quaternion nextQuat = Math::Quaternion::Slerp(currentQuat, m_targetRotation, deltaTime * m_rotationSpeed);

		//回転させる
		Math::Vector3 nextEuler = nextQuat.ToEuler();
		nextEuler.x = DirectX::XMConvertToDegrees(nextEuler.x);
		nextEuler.y = DirectX::XMConvertToDegrees(nextEuler.y);
		nextEuler.z = DirectX::XMConvertToDegrees(nextEuler.z);
		transform->SetRot(nextEuler);

		//ほぼ目標回転に達したら終了(クォータニオンの比較はドット積で行うのが一般的)
		if (abs(currentQuat.Dot(m_targetRotation)) > 0.9999f)
		{
			//誤差をなくすために目標値で確定
			Math::Vector3 finalEuler = m_targetRotation.ToEuler();
			finalEuler.x = DirectX::XMConvertToDegrees(finalEuler.x);
			finalEuler.y = DirectX::XMConvertToDegrees(finalEuler.y);
			finalEuler.z = DirectX::XMConvertToDegrees(finalEuler.z);
			transform->SetRot(finalEuler);
			m_isRotating = false;
			m_cooldownTimer = 0.5f;
		}
	}
}

void RotatingBlockComponent::OnCollision(const CollisionInfo& info)
{
	if (m_isRotating || m_cooldownTimer > 0.0f)return;

	if (info.otherObject && info.contactNormal.y > 0.7f)
	{
		m_isRotating = true;

		//追加する回転量を計算
		Math::Quaternion rotationStep;
		rotationStep = rotationStep.CreateFromAxisAngle(m_rotationAxis, DirectX::XMConvertToRadians(m_rotationAmount));

		//新しい目標回転をセット
		m_targetRotation = m_targetRotation * rotationStep;
	}
}

void RotatingBlockComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Rotating Block Compoent", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool itemDeactivated = false;

		//--回転軸--
		ImGui::DragFloat3("Rotation Axis", &m_rotationAxis.x, 0.01f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--回転量--
		ImGui::DragFloat("Rotation Amount", &m_rotationAmount, 1.0f, 0.0f, 360.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--回転スピード--
		ImGui::DragFloat("Rotation Speed", &m_rotationSpeed, 0.1f, 0.1f, 20.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//いずれかのウィジェットウィジェットのドラッグが終了した瞬間
		if (itemDeactivated)
		{
			RequestTransformChangeCommand();
		}
	}
}

void RotatingBlockComponent::RequestTransformChangeCommand()
{
	if (auto viewModel = m_wpViewModel.lock())
	{
		if (auto idComp = m_owner->GetComponent<IdComponent>())
		{
			viewModel->UpdateStateFromGameObject(m_owner->shared_from_this());
		}
	}
}
