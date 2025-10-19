#include "ResultScene.h"
#include"../GameScene/GameManager/GameManager.h"
#include"../Src/Framework/Component/TimerComponent/TimerComponent.h"
#include"../Src/Framework/Component/TransformComponent/TransformComponent.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/BackgroundComponent/BackgroundComponent.h"

void ResultScene::Init()
{
	m_finalTime = GameManager::Instance().GetFinalTime();
	m_clearTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/UI/StageClear.png");

	m_timerObject = std::make_shared<GameObject>();
	auto timerComp = m_timerObject->AddComponent<TimerComponent>();
	timerComp->SetElapsedTime(m_finalTime);
	auto timerTransform = m_timerObject->AddComponent<TransformComponent>();
	m_timerPos = { -200.0f,0.0f };
	timerTransform->SetPos({ m_timerPos.x,m_timerPos.y,0.0f });
	timerTransform->SetScale({ 1.5f,1.5f,1.5f });
	m_timerObject->Init();
	SceneManager::Instance().SetMode(SceneManager::SceneMode::UI);

	//背景
	auto backgroundObj = std::make_shared<GameObject>();
	backgroundObj->SetName("Background");
	backgroundObj->AddComponent<BackgroundComponent>();
	AddObject(backgroundObj);
	backgroundObj->Init();
}

void ResultScene::SceneUpdate()
{
	float deltatime = Application::Instance().GetDeltaTime();
	float fadeSpeed = 1.5f;

	if (m_texAlpha < 1.0f)
	{
		m_texAlpha += fadeSpeed * deltatime;
	}

	if (m_texAlpha >= 0.5f && m_uiAlpha < 1.0f)
	{
		m_uiAlpha += fadeSpeed * deltatime;
	}

	m_texAlpha = std::min(m_texAlpha, 1.0f);
	m_uiAlpha = std::min(m_uiAlpha, 1.0f);
}

void ResultScene::DrawSprite()
{
	BaseScene::DrawSprite();

	if (m_timerObject)
	{
		m_timerObject->DrawSprite();
	}
}

void ResultScene::Draw()
{
	BaseScene::Draw();
	DrawClearWindow();
	DrawButtonWindow();
}

void ResultScene::DrawClearWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_texAlpha);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

	//画面上の方の中央に設置
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->GetCenter().x, viewport->WorkPos.y + 200), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("ClearText", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	if (m_clearTex)
	{
		ImTextureID texID = m_clearTex->WorkSRView();
		ImVec2 texSize = ImVec2((float)m_clearTex->GetInfo().Width, (float)m_clearTex->GetInfo().Height);
		ImGui::Image(texID, texSize);
	}

	ImGui::End();

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();
}

void ResultScene::DrawButtonWindow()
{
	if (m_uiAlpha <= 0.0f)return;
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_uiAlpha);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.4f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.5f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.4f, 0.6f, 1.0f));

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	{//ボタン
		ImGui::SetNextWindowPos(ImVec2(viewport->GetCenter().x, viewport->WorkPos.y + 500), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(200, 0));
		ImGui::Begin("ResultButtons", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button("Retry", ImVec2(-1, 0)))
		{
			//現在のステージをもう一度読み込む
			const auto& gm = GameManager::Instance();
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Game);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
		}
		ImGui::Spacing();
		if (ImGui::Button("Stage Select", ImVec2(-1, 0)))
		{
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::StageSelect);
		}
		ImGui::Spacing();
		if (ImGui::Button("Back To Title", ImVec2(-1, 0)))
		{
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Title);
		}
		ImGui::End();
	}
	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar();
}
