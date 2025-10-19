#include "PlayerStatsComponent.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void PlayerStatsComponent::OnInspect()
{
	//このコンポーネントのプロパティをImGuiで表示・編集する
	if (ImGui::CollapsingHeader("Player Stats Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::SliderInt("HP", &m_hp, 0, m_maxHp);

		float hpRatio = (float)m_hp / m_maxHp;
		ImGui::ProgressBar(hpRatio, ImVec2(-1, 0));

		bool valueChanged = false;

		valueChanged |= ImGui::DragFloat3("InitialPos", &m_initialPos.x, 0.1f);
	}
}

void PlayerStatsComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("PlayerStatsComponent")) return;

	const auto& stageData = data.at("PlayerStatsComponent");

	if (stageData.contains("initialPos"))
	{
		JsonHelper::GetVector3(stageData, "initialPos", m_initialPos, m_initialPos);
	}
}

nlohmann::json PlayerStatsComponent::ToJson() const
{
	nlohmann::json j;
	j["initialPos"] = { m_initialPos.x,m_initialPos.y,m_initialPos.z };
	return j;
}
