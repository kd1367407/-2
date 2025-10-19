#include "TutorialTriggerComponent.h"
#include"../GameObject.h"
#include"../Src/Application/Scene/GameScene/GameScene.h"
#include"../ICollisionReceiver.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void TutorialTriggerComponent::Awake()
{
	m_hasTriggered = false;
}

void TutorialTriggerComponent::OnTriggerEnter(const CollisionInfo& info)
{
	if (m_hasTriggered || !info.otherObject)return;

	if (info.otherObject->GetName() == "Player")
	{
		//描画依頼
		if (auto scene = dynamic_cast<GameScene*>(SceneManager::Instance().GetCurrentScene()))
		{
			scene->ShowTutorialHint(m_tutorialText, m_tutorialImagePath, m_tutorialBlockName);
		}

		m_hasTriggered = true;
	}
}

void TutorialTriggerComponent::OnInspect()
{

	if (ImGui::CollapsingHeader("TutorialTrigger Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		char BlockNameBuffer[256] = {};
		strcpy_s(BlockNameBuffer, sizeof(BlockNameBuffer), m_tutorialBlockName.c_str());
		if (ImGui::InputText("BlockName", BlockNameBuffer, sizeof(BlockNameBuffer)))
		{
			m_tutorialBlockName = BlockNameBuffer;
		}

		char textBuffer[256] = {};
		strcpy_s(textBuffer, sizeof(textBuffer), m_tutorialText.c_str());
		if (ImGui::InputTextMultiline("Text", textBuffer, sizeof(textBuffer)))
		{
			m_tutorialText = textBuffer;
		}

		char imagePathBuffer[256] = {};
		strcpy_s(imagePathBuffer, sizeof(imagePathBuffer), m_tutorialImagePath.c_str());
		if (ImGui::InputText("Image Path", imagePathBuffer, sizeof(imagePathBuffer)))
		{
			m_tutorialImagePath = imagePathBuffer;
		}
	}
}

nlohmann::json TutorialTriggerComponent::ToJson() const
{
	nlohmann::json j;
	j["BlockName"] = m_tutorialBlockName;
	j["Text"] = m_tutorialText;
	j["ImagePath"] = m_tutorialImagePath;

	return j;
}

void TutorialTriggerComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("TutorialTriggerComponent"))return;

	const auto& tutorialTriggerData = data.at("TutorialTriggerComponent");

	m_tutorialBlockName = JsonHelper::GetString(tutorialTriggerData, "BlockName", "");
	m_tutorialText = JsonHelper::GetString(tutorialTriggerData, "Text", "");
	m_tutorialImagePath = JsonHelper::GetString(tutorialTriggerData, "ImagePath", "N/A");
}
