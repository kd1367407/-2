#include "MovingBlockComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../RigidbodyComponent/RigidbodyComponent.h"
#include"../IdComponent/IdComponent.h"
#include"../Src/Application/main.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"
#include"../Src/Application/GameViewModel.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../../ImGuizmo/ImGuizmo.h"
#include"../../Editor/EditorGizmoContext.h"
#include"../Src/Application/Scene/GameScene/GameScene.h"

void MovingBlockComponent::Awake()
{
}

void MovingBlockComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();
}

void MovingBlockComponent::Update()
{
	if (!m_isActive)return;
	if (!m_transform)return;

	//進捗度を更新
	float speed = 1.0f / m_duration;
	float deltaTime = Application::Instance().GetDeltaTime();
	if (m_isRevarse)
	{
		m_progress -= speed * deltaTime;
	}
	else
	{
		m_progress += speed * deltaTime;
	}

	//折り返し処理
	if (m_progress >= 1.0f)
	{
		m_progress = 1.0f;
		m_isRevarse = true;
	}
	else if (m_progress <= 0.0f)
	{
		m_progress = 0.0f;
		m_isRevarse = false;
	}

	//イージングを適用した進捗度を計算
	float easedProgress = EaseInOutSine(m_progress);

	//新しい座標を計算してTransformに適用
	Math::Vector3 newPos = Math::Vector3::Lerp(m_startPos, m_endPos, easedProgress);
	m_transform->SetPos(newPos);
}

void MovingBlockComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("MovingBlockComponent"))return;

	const auto& MovingBlockData = data.at("MovingBlockComponent");

	m_isActive = JsonHelper::GetBool(MovingBlockData, "active", false);
	m_duration = JsonHelper::GetFloat(MovingBlockData, "duration", 2.0f);

	if (MovingBlockData.contains("startPos"))
	{
		JsonHelper::GetVector3(MovingBlockData, "startPos", m_startPos, { 0,2.0,0 });
	}

	if (MovingBlockData.contains("endPos"))
	{
		const auto& endPosValue = MovingBlockData["endPos"];
		//値が文字列かチェック
		if (endPosValue.is_string())
		{
			std::string endPosStr = endPosValue.get<std::string>();
			
			//文字列の先頭がstartPos(で始まっているか
			if (endPosStr.rfind("startPos(", 0) == 0)
			{
				Math::Vector3 offset;
				sscanf_s(endPosStr.c_str(), "startPos(%f,%f,%f)", &offset.x, &offset.y, &offset.z);

				m_endPos = m_startPos + offset;
			}
		}
		else if (endPosValue.is_array())//値が配列の場合
		{
			JsonHelper::GetVector3(MovingBlockData, "endPos", m_endPos, m_endPos);
		}
	}
}

nlohmann::json MovingBlockComponent::ToJson() const
{
	nlohmann::json j;
	
	j["active"] = m_isActive;
	j["duration"] = m_duration;
	j["startPos"] = { m_startPos.x,m_startPos.y,m_startPos.z };
	j["endPos"] = { m_endPos.x,m_endPos.y,m_endPos.z };

	return j;
}

void MovingBlockComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Moving Block Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool itemDeactivated = false;
		ImGui::DragFloat3("Start Position", &m_startPos.x);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
		
		ImGui::DragFloat3("End Position", &m_endPos.x);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();
		
		ImGui::DragFloat("Duration (seconds)", &m_duration, 0.1f, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		if (itemDeactivated)
		{
			RequestStateChangeCommand();
		}
	}
}

bool MovingBlockComponent::OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene)
{
	bool isUsingStart = false;
	bool isUsingEnd = false;

	//--startPos--
	ImGuizmo::PushID("startPosGizmo");

	Math::Matrix startMat = Math::Matrix::CreateTranslation(m_startPos);

	ImGuizmo::Manipulate(
		(float*)context.viewMat, (float*)context.projMat,
		ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&startMat
	);

	if (ImGuizmo::IsUsing())
	{
		if (!m_isStartGizmoDragging)
		{
			isUsingStart = true;
			m_isStartGizmoDragging = true;
		}
		auto newPos = startMat.Translation();
		SetStartPos(newPos);
	}
	else if (m_isStartGizmoDragging)
	{
		RequestStateChangeCommand();
		m_isStartGizmoDragging = false;
	}

	ImGuizmo::PopID();

	//--endPos--
	ImGuizmo::PushID("endPosGizmo");

	Math::Matrix endPosMat = Math::Matrix::CreateTranslation(m_endPos);

	ImGuizmo::Manipulate(
		(float*)context.viewMat, (float*)context.projMat,
		ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&endPosMat
	);

	if (ImGuizmo::IsUsing())
	{
		if (!m_isEndGizmoDragging)
		{
			isUsingEnd = true;
			m_isEndGizmoDragging = true;
		}
		auto newPos = endPosMat.Translation();
		SetEndPos(newPos);
	}
	else if (m_isEndGizmoDragging)
	{
		RequestStateChangeCommand();
		m_isEndGizmoDragging = false;
	}

	ImGuizmo::PopID();

	if (auto debugWire = scene.GetDebugWire())
	{
		debugWire->AddDebugLine(m_startPos, m_endPos, kGreenColor);
	}

	return isUsingStart || isUsingEnd;
}

void MovingBlockComponent::RequestStateChangeCommand()
{
	if (auto viewModel = m_wpViewModel.lock())
	{
		if (auto idComp = m_owner->GetComponent<IdComponent>())
		{
			viewModel->UpdateStateFromGameObject(m_owner->shared_from_this());
		}
	}
}
