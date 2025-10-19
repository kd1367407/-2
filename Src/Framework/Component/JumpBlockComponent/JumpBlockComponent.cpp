#include "JumpBlockComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../RigidbodyComponent/RigidbodyComponent.h"
#include"../GravityComponent/GravityComponent.h"
#include"../Src/Application/main.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../../Editor/EditorGizmoContext.h"
#include"../Src/Application/Scene/GameScene/GameScene.h"
#include"../IdComponent/IdComponent.h"

void JumpBlockComponent::Awake()
{
}

void JumpBlockComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();
}

void JumpBlockComponent::OnCollision(const CollisionInfo& info)
{
	if (!m_transform || !info.otherObject)return;

	if (info.contactNormal.y > 0.7f)
	{
		if (auto rigid = info.otherObject->GetComponent<RigidbodyComponent>())
		{
			if (rigid->m_type == RigidbodyType::Dynamic)
			{
				rigid->SetVelocity(Math::Vector3::Zero); // 既存の速度をリセット
				rigid->AddForce(m_jumpDirection * m_jumpForce);
			}
		}
	}
}

void JumpBlockComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Jump Block Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool valueChanged = false;

		valueChanged |= ImGui::DragFloat3("Jump Direction", &m_jumpDirection.x, 0.01f);
		valueChanged |= ImGui::DragFloat("Jump Force", &m_jumpForce, 1.0f, 0.0f, 1000.0f);

		// いずれかのUIのドラッグが終了した瞬間
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			RequestStateChangeCommand();
		}
	}
}

void JumpBlockComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("JumpBlockComponent"))return;

	const auto& JumpBLockData = data.at("JumpBlockComponent");

	if (JumpBLockData.contains("direction"))
	{
		JsonHelper::GetVector3(JumpBLockData, "direction", m_jumpDirection, { 0.0f,4.0f,0.0f });
	}

	if (JumpBLockData.contains("force"))
	{
		m_jumpForce = JsonHelper::GetFloat(JumpBLockData, "force", 100.0f);
	}
}

nlohmann::json JumpBlockComponent::ToJson() const
{
	nlohmann::json j;

	j["direvtion"] = { m_jumpDirection.x,m_jumpDirection.y,m_jumpDirection.z };
	j["force"] = m_jumpForce;

	return j;
}

bool JumpBlockComponent::OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene)
{
	bool isTransformGizmoUse = false;
	if (auto transform = m_owner->GetComponent<TransformComponent>())
	{
		isTransformGizmoUse = transform->OnDrawGizmos(context, scene);
	}

	bool isTargetGizmoUse = false;

	ImGuizmo::PushID("targetGizmo");

	Math::Matrix targetMat = Math::Matrix::CreateTranslation(GetTargetPos());

	bool isObjectDragActive = false;

	ImGuizmo::Manipulate(
		(float*)context.viewMat, (float*)context.projMat,
		ImGuizmo::TRANSLATE, ImGuizmo::WORLD, (float*)&targetMat
	);

	if (ImGuizmo::IsUsing())
	{
		if (!m_isTargetGizmoDragging)
		{
			isTargetGizmoUse = true;
			m_isTargetGizmoDragging = true;
		}
		auto newPos = targetMat.Translation();
		SetTargetPos(newPos);
	}
	else if (m_isTargetGizmoDragging)
	{
		RequestStateChangeCommand();
		m_isTargetGizmoDragging = false;
	}

	ImGuizmo::PopID();

	if (auto debugWire = scene.GetDebugWire())
	{
		debugWire->AddDebugLine(GetOwnerPos(), GetTargetPos(), kGreenColor);
	}

	return isTransformGizmoUse || isTargetGizmoUse;
}

void JumpBlockComponent::RequestStateChangeCommand()
{
	if (auto viewModel = m_wpViewModel.lock())
	{
		if (auto idComp = m_owner->GetComponent<IdComponent>())
		{
			viewModel->UpdateStateFromGameObject(m_owner->shared_from_this());
		}
	}
}

void JumpBlockComponent::SetTargetPos(const Math::Vector3& pos)
{
	if (m_transform)
	{
		//ワールド座標をローカル座標に変換して保存
		Math::Matrix invOwnerMat = m_transform->GetMatrix().Invert();
		m_jumpDirection = Math::Vector3::Transform(pos, invOwnerMat);
	}
}

const Math::Vector3 JumpBlockComponent::GetTargetPos()const
{
	//ローカル座標をワールド座標に変換して返す
	if (m_transform)
	{
		return Math::Vector3::Transform(m_jumpDirection, m_transform->GetMatrix());
	}
	return Math::Vector3::Zero;
}

const Math::Vector3& JumpBlockComponent::GetOwnerPos() const
{
	if (m_transform) return m_transform->GetPos();
	return Math::Vector3::Zero; // 安全対策
}
