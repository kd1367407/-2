#include "ScalingBlockComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/main.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../IdComponent/IdComponent.h"
#include"../RigidbodyComponent/RigidbodyComponent.h"
#include"../ColliderComponent/ColliderComponent.h"

void ScalingBlockComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("ScalingBlockComponent"))return;

	const auto& scalingBlockData = data.at("ScalingBlockComponent");

	JsonHelper::GetVector3(scalingBlockData, "scale_axis", m_scaleAxis, m_scaleAxis);
	m_scaleAmount = JsonHelper::GetFloat(scalingBlockData, "scale_amount", m_scaleAmount);
	m_scaleSpeed = JsonHelper::GetFloat(scalingBlockData, "scale_speed", m_scaleSpeed);
}

void ScalingBlockComponent::Awake()
{
}

void ScalingBlockComponent::Start()
{
	m_wpTransform = m_owner->GetComponent<TransformComponent>();

	//開始時のスケールを基本スケールとして記憶
	if (auto transform = m_wpTransform.lock())
	{
		m_baseScale = transform->GetScale();
		m_lastScale = m_baseScale;
	}

	if (auto collider = m_owner->GetComponent<ColliderComponent>())
	{
		if (auto shape = collider->GetShape())
		{
			m_colliderFullHeight = shape->GetBoundingBox().Extents.y * 2.0f;
		}
	}
}

void ScalingBlockComponent::Update()
{
	auto transform = m_wpTransform.lock();
	if (!transform)return;

	float deltatime = Application::Instance().GetDeltaTime();

	if (m_playerOffTimer > 0.0f)
	{
		m_playerOffTimer -= deltatime;
	}

	if (m_playerOffTimer <= 0.0f)
	{
		m_isPlayerOnTop = false;
		m_wpPlayer.reset(); // プレイヤーへのポインタもクリア
	}

	m_elapsedTime += deltatime * m_scaleSpeed;

	//sin波計算
	float sinWave = sin(m_elapsedTime);

	//新しいスケール計算
	Math::Vector3 newScale = m_baseScale + (m_scaleAxis * m_scaleAmount * sinWave);

	//スケールが0に以下にならないようにクランプ
	newScale.x = std::max(newScale.x, 0.5f);
	newScale.y = std::max(newScale.y, 0.5f);
	newScale.z = std::max(newScale.z, 0.5f);

	float scaleDeltaY = newScale.y - m_lastScale.y;

	if (m_isPlayerOnTop && scaleDeltaY < 0.0f)
	{
		if (auto player = m_wpPlayer.lock())
		{
			if (auto playerRigid = player->GetComponent<RigidbodyComponent>())
			{
				float moveAmount = scaleDeltaY * m_colliderFullHeight;
				playerRigid->m_additionalMovement.y += moveAmount;
			}
		}
	}

	transform->SetScale(newScale);

	m_lastScale = newScale;
}

void ScalingBlockComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Scaling Block Compoent", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool itemDeactivated = false;

		ImGui::DragFloat3("Scale Axis", &m_scaleAxis.x, 0.01f, 0.0f, 1.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		ImGui::DragFloat("Scale Amount", &m_scaleAmount, 0.05f, 0.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		ImGui::DragFloat("Scale Speed", &m_scaleSpeed, 0.05f, 0.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		if (itemDeactivated)
		{
			RequestTransformChangeCommand();
		}
	}
}

void ScalingBlockComponent::OnCollision(const CollisionInfo& info)
{
	if (info.otherObject && info.otherObject->GetName() == "Player" && info.contactNormal.y > 0.7f)
	{
		m_isPlayerOnTop = true;
		m_playerOffTimer = 0.1f;
		m_wpPlayer = info.otherObject;
	}
}

void ScalingBlockComponent::RequestTransformChangeCommand()
{
	if (auto viewModel = m_wpViewModel.lock())
	{
		if (auto idComp = m_owner->GetComponent<IdComponent>())
		{
			viewModel->UpdateStateFromGameObject(m_owner->shared_from_this());
		}
	}
}
