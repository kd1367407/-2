#include "ImGuiComponent.h"
#include"../GameObject.h"

void ImGuiComponent::DrawImGui()
{
	if (!m_owner)return;

	if (ImGui::CollapsingHeader(m_owner->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (const auto& comp : m_owner->GetComponents())
		{
			comp->OnInspect();
		}
	}
}
