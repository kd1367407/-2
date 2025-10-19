#include "SinkingBlockComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../IdComponent/IdComponent.h"
#include"../../ImGuizmo/ImGuizmo.h"
#include"../../Editor/EditorGizmoContext.h"
#include"../Src/Application/Scene/SceneManager.h"

void SinkingBlockComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("SinkingBlockComponent"))return;

	const auto& SinkBlockData = data.at("SinkingBlockComponent");

	JsonHelper::GetVector3(SinkBlockData, "initial_pos", m_initialPos, m_initialPos);
	m_maxSinkDistance = JsonHelper::GetFloat(SinkBlockData, "max_sink_distance", m_maxSinkDistance);
	m_acceleration = JsonHelper::GetFloat(SinkBlockData, "acceleration", m_acceleration);
	m_riseSpeed = JsonHelper::GetFloat(SinkBlockData, "rise_speed", m_riseSpeed);
}

void SinkingBlockComponent::Awake()
{
}

void SinkingBlockComponent::Start()
{
	m_wpTransform = m_owner->GetComponent<TransformComponent>();

	//Configureで読み込めていない場合のために配置位置を初期位置とする
	if (auto transform = m_wpTransform.lock())
	{
		m_initialPos = transform->GetPos();
	}
}

void SinkingBlockComponent::Update()
{
	auto transform = m_wpTransform.lock();
	if (!transform)return;
	float deltatime = Application::Instance().GetDeltaTime();

	if (SceneManager::Instance().GetCurrentMode() != SceneManager::SceneMode::Game)
	{
		if (transform->GetPos() != m_initialPos)
		{
			transform->SetPos(m_initialPos);
		}
	}

	//離脱検知タイマー更新
	if (m_playerOffTimer > 0.0f)
	{
		m_playerOffTimer -= deltatime;
	}

	if (m_playerOffTimer <= 0.0f)
	{
		m_isPlayerOnTop = false;
	}

	Math::Vector3 currentPos = transform->GetPos();

	//--playerが乗っている時--
	if (m_isPlayerOnTop)
	{
		Math::Vector3 sunkPos = m_initialPos;
		sunkPos.y -= m_maxSinkDistance;

		if (currentPos.y > sunkPos.y)
		{
			m_currentSpeed += m_acceleration * deltatime;
			currentPos.y -= m_currentSpeed * deltatime;
			
			if (currentPos.y < sunkPos.y)
			{
				currentPos.y = sunkPos.y;
			}
			transform->SetPos(currentPos);
		}
	}

	//--playerが乗っていない時--
	else
	{
		m_currentSpeed = 0;
		if (currentPos.y < m_initialPos.y)
		{
			currentPos.y += m_riseSpeed * deltatime;

			if (currentPos.y > m_initialPos.y)
			{
				currentPos.y = m_initialPos.y;
			}
			transform->SetPos(currentPos);
		}
	}
}

void SinkingBlockComponent::OnCollision(const CollisionInfo& info)
{
	if (info.otherObject && info.otherObject->GetName() == "Player" && info.contactNormal.y > 0.7f)
	{
		m_isPlayerOnTop = true;
		m_playerOffTimer = 0.1f;
	}
}

void SinkingBlockComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Sinking Block Compoent", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool itemDeactivated = false;

		//--初期座標--
		ImGui::DragFloat3("Initial Pos", &m_initialPos.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--最大下降距離--
		ImGui::DragFloat("Max Sink Distance", &m_maxSinkDistance, 0.1f, 0.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--加速度--
		ImGui::DragFloat("Acceleration", &m_acceleration, 0.1f, 0.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--上昇スピード--
		ImGui::DragFloat("Rise Speed", &m_riseSpeed, 0.1f, 0.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//いずれかのウィジェットウィジェットのドラッグが終了した瞬間
		if (itemDeactivated)
		{
			RequestTransformChangeCommand();
		}
	}
}

nlohmann::json SinkingBlockComponent::ToJson() const
{
	nlohmann::json j;
	j["initial_pos"] = { m_initialPos.x,m_initialPos.y,m_initialPos.z };
	j["max_sink_distance"] = m_maxSinkDistance;
	j["acceleration"] = m_acceleration;
	j["rise_speed"] = m_riseSpeed;

	return j;
}

bool SinkingBlockComponent::OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene)
{
	bool isUsingInitial = false;

	ImGuizmo::PushID("initialPosGizmo");

	Math::Matrix initialMat = Math::Matrix::CreateTranslation(m_initialPos);

	ImGuizmo::Manipulate(
		(float*)context.viewMat, (float*)context.projMat,
		ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&initialMat
	);

	if (ImGuizmo::IsUsing())
	{
		if (!m_isInitialGizmoDragging)
		{
			isUsingInitial = true;
			m_isInitialGizmoDragging = true;
		}
		auto newPos = initialMat.Translation();
		SetInitialPos(newPos);
	}
	else if (m_isInitialGizmoDragging)
	{
		RequestTransformChangeCommand();
		m_isInitialGizmoDragging = false;
	}

	ImGuizmo::PopID();

	return isUsingInitial;
}

void SinkingBlockComponent::RequestTransformChangeCommand()
{
	if (auto viewModel = m_wpViewModel.lock())
	{
		if (auto idComp = m_owner->GetComponent<IdComponent>())
		{
			viewModel->UpdateStateFromGameObject(m_owner->shared_from_this());
		}
	}
}
