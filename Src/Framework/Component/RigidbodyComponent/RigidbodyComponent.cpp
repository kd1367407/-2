#include "RigidbodyComponent.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/System/PhysicsSystem.h"
#include"../GameObject.h"
#include"../GravityComponent/GravityComponent.h"
#include"../JumpBlockComponent/JumpBlockComponent.h"
#include"../SlipperyComponent/SlipperyComponent.h"

void RigidbodyComponent::Awake()
{
}

void RigidbodyComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();

	PhysicsSystem::Instance().RegisterRigidbody(std::dynamic_pointer_cast<RigidbodyComponent>(shared_from_this()));
}

void RigidbodyComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("RigidbodyComponent"))return;

	const auto& rigidData = data.at("RigidbodyComponent");

	if (rigidData.contains("type"))
	{
		std::string typeStr = rigidData["type"].get<std::string>();

		if (typeStr == "dynamic")
		{
			m_type = RigidbodyType::Dynamic;
		}
		else if (typeStr == "kinematic")
		{
			m_type = RigidbodyType::Kinematic;
		}
		else if (typeStr == "static")
		{
			m_type = RigidbodyType::Static;
		}
	}
}

nlohmann::json RigidbodyComponent::ToJson() const
{
	nlohmann::json j;
	std::string typeStr = "";

	if (m_type == RigidbodyType::Dynamic)
	{
		typeStr = "dynamic";
	}
	else if (m_type == RigidbodyType::Kinematic)
	{
		typeStr = "kinematic";
	}
	else if (m_type == RigidbodyType::Static)
	{
		typeStr = "static";
	}

	j["type"] = typeStr;

	return j;
}

void RigidbodyComponent::SetGround(const std::shared_ptr<GameObject>& groundObject)
{
	if (groundObject)
	{
		auto groundT = groundObject->GetComponent<TransformComponent>();

		//新しく床に乗ったか、乗る床が変わった場合
		if (m_groundTransform.lock() != groundT)
		{
			m_groundTransform = groundT;
			if (groundT)
			{
				m_groundLastPos = groundT->GetPos();
			}
		}

		//滑る床か
		if (auto slipperyComp = groundObject->GetComponent<SlipperyComponent>())
		{
			m_isOnSlipperySurface = true;
			m_surfaceDragValue = slipperyComp->GetDragCoefficient();
		}
		else
		{
			m_isOnSlipperySurface = false;
		}
	}
	else//空中にいる場合
	{
		m_groundTransform.reset();
		m_isOnSlipperySurface = false;
	}
}

void RigidbodyComponent::OnInspect()
{
	//このコンポーネントのプロパティをImGuiで表示、編集する
	if (ImGui::CollapsingHeader("RigidBody Compoent", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char* typeStr = "Unknown";
		switch (m_type)
		{
		case RigidbodyType::Dynamic:
			typeStr = "Dynamic";
			break;
		case RigidbodyType::Kinematic:
			typeStr = "Kinematic";
			break;
		case RigidbodyType::Static:
			typeStr = "Static";
			break;
		}
		ImGui::Text("Type: %s", typeStr);

		ImGui::DragFloat3("Velocity", &m_velocity.x, 0.1f);
		ImGui::DragFloat3("Force", &m_force.x, 0.1f);
		ImGui::DragFloat("Mass", &m_mass, 0.1f);
	}
}
