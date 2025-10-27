#include "TitleScene.h"
#include"../SceneManager.h"
#include"../GameScene/GameManager/GameManager.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/BackgroundComponent/BackgroundComponent.h"
#include"../../main.h"
#include"../../SettingsManager/SettingsManager.h"

void TitleScene::Init()
{
	SceneManager::Instance().SetMode(SceneManager::SceneMode::UI);
	m_showTemplateSelect = false;

	m_titleTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/UI/Title2.png");
	m_playButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/PlayButtonKari.png");
	m_createButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/CreateButtonKari.png");

	m_playButtonPos = { 0,0 };

	//背景
	auto backgroundObj = std::make_shared<GameObject>();
	backgroundObj->SetName("Background");
	backgroundObj->AddComponent<BackgroundComponent>();
	AddObject(backgroundObj);
	backgroundObj->Init();

	m_spBGM = KdAudioManager::Instance().Play("Asset/Sound/TitleBGM.wav", true, 1.0f);
}

void TitleScene::SceneUpdate()
{
	float deltatime = Application::Instance().GetDeltaTime();
	float fadeSpeed = 1.0f;
	auto& fader = SceneManager::Instance().GetFader();

	if (m_titleAlpha < 1.0f)
	{
		if (!fader.IsFadeing())
		{
			m_titleAlpha += fadeSpeed * deltatime;
		}
	}

	if (m_titleAlpha >= 0.5f && m_buttonAlpha < 1.0f) 
	{
		m_buttonAlpha += fadeSpeed * deltatime;
	}

	m_titleAlpha = std::min(m_titleAlpha, 1.0f);
	m_buttonAlpha = std::min(m_buttonAlpha, 1.0f);


	long buttonX = m_playButtonPos.x+500;
	long buttonY = m_playButtonPos.y+300;
	long buttonWidth = m_playButtonTex->GetInfo().Width;
	long buttonHeight = m_playButtonTex->GetInfo().Height;

	Math::Rectangle buttonAngle(buttonX, buttonY, buttonWidth, buttonHeight);

	POINT mousePos;
	GetCursorPos(&mousePos);

	bool isMouseOver = buttonAngle.Contains(static_cast<long>(mousePos.x), static_cast<long>(mousePos.y));

	if (isMouseOver && KdInputManager::Instance().IsPress("Select"))
	{
		GameManager::Instance().SetLoadMode(GameManager::LoadMode::Play);
		SceneManager::Instance().ChangeScene(SceneManager::SceneType::StageSelect);
	}
}

void TitleScene::Draw()
{
	BaseScene::Draw();
	/*DrawTitleWindow();
	DrawButtonWindow();*/
}

void TitleScene::DrawSprite()
{
	BaseScene::DrawSprite();
	DrawTitleWindow();
	DrawNormalButton();
}

void TitleScene::Release()
{
	if (m_spBGM && m_spBGM->IsPlaying())
	{
		m_spBGM->Stop();
	}
	m_spBGM = nullptr;
}

void TitleScene::DrawTitleWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_titleAlpha);//透明度はm_titleAlphaを使う(ウィンドウ、テキストなど全て)
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

	//画面上の方の中央に設置
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->GetCenter().x, viewport->WorkPos.y + 300), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("TitleText", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	if (m_titleTex)
	{
		ImTextureID texID = m_titleTex->WorkSRView();
		ImVec2 texSize = ImVec2((float)m_titleTex->GetInfo().Width, (float)m_titleTex->GetInfo().Height);
		ImGui::Image(texID, texSize);
	}

	ImGui::End();

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();

	/*Math::Color color = { 1,1,1,m_titleAlpha };
	if (m_titleTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_titleTex.get(), 0, 0, nullptr, &color);
	}*/
}

void TitleScene::DrawButtonWindow()
{
	if (m_buttonAlpha <= 0.0f) return;

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_buttonAlpha);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button,		  ImVec4(0.1f, 0.2f, 0.4f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.5f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.3f, 0.4f, 0.6f, 1.0f));

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->GetCenter().x, viewport->WorkPos.y+500), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(300, 0));

	ImGui::Begin("MainMenu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);

	//新規作成のテンプレート選択UIが表示されている場合
	if (m_showTemplateSelect)
	{
		if (ImGui::Button("Foundation 1", ImVec2(-1, 0)))
		{
			auto& gm = GameManager::Instance();
			gm.SetLoadMode(GameManager::LoadMode::CreateNew);
			gm.SetNextStage("Asset/Data/Stages/Template01.json", "New Stage(Foundation 1)");
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Create);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
		}
		if (ImGui::Button("Foundation 2", ImVec2(-1, 0)))
		{
			auto& gm = GameManager::Instance();
			gm.SetLoadMode(GameManager::LoadMode::CreateNew);
			gm.SetNextStage("Asset/Data/Stages/Template02.json", "New Stage(Foundation 2)");
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Create);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
		}
		if (ImGui::Button("Foundation 3", ImVec2(-1, 0)))
		{
			auto& gm = GameManager::Instance();
			gm.SetLoadMode(GameManager::LoadMode::CreateNew);
			gm.SetNextStage("Asset/Data/Stages/Template03.json", "New Stage(Foundation 3)");
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Create);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
		}

		ImGui::Spacing();
		if (ImGui::Button("Back", ImVec2(-1, 0)))
		{
			m_showTemplateSelect = false;
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
		}
	}
	//サウンドボリューム調整
	else if (m_showVolume)
	{
		float seVol = KdAudioManager::Instance().GetSEVolume();
		if (ImGui::SliderFloat("SE Volume", &seVol, 0.0f, 1.0f))
		{
			KdAudioManager::Instance().SetSEVolume(seVol);
		}

		float bgmVol = KdAudioManager::Instance().GetBGMVolume();
		if (ImGui::SliderFloat("BGM Volume", &bgmVol, 0.0f, 1.0f))
		{
			KdAudioManager::Instance().SetBGMVolume(bgmVol);
		}

		ImGui::Spacing();
		if (ImGui::Button("Back", ImVec2(-1, 0)))
		{
			m_showVolume = false;
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);

			//書き込み可能なjsonオブジェクトを取得
			nlohmann::json& settings = SettingsManager::Instance().WorkGameSetting();

			//"volume_settings"の値を上書き
			settings["volume_settings"]["SE"] = KdAudioManager::Instance().GetSEVolume();
			settings["volume_settings"]["BGM"] = KdAudioManager::Instance().GetBGMVolume();

			//save指示(変更したjsonをファイルに書き出し)
			SettingsManager::Instance().SaveGameSetting();
		}
	}
	//通常のタイトルメニュー
	else
	{
		if (ImGui::Button("Play", ImVec2(-1, 0)))
		{
			GameManager::Instance().SetLoadMode(GameManager::LoadMode::Play);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::StageSelect);
		}

		ImGui::Spacing();

		if (ImGui::Button("Create", ImVec2(-1, 0)))
		{
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
			ImGui::OpenPopup("CreatePopup");
		}

		//出現座標をボタンの下に
		ImVec2 buttonMin = ImGui::GetItemRectMin();
		ImGui::SetNextWindowPos(ImVec2(buttonMin.x, buttonMin.y + 30));

		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.1f, 0.1f, 0.2f, 0.7f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.8f, 0.8f, 0.8f, 0.4f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);

		//createボタンのポップアップ
		if (ImGui::BeginPopup("CreatePopup"))
		{
			if (ImGui::MenuItem("Edit Existing Stage"))
			{
				GameManager::Instance().SetLoadMode(GameManager::LoadMode::Edit);
				SceneManager::Instance().ChangeScene(SceneManager::SceneType::StageSelect);
			}

			if (ImGui::MenuItem("Create New Stage"))
			{
				m_showTemplateSelect = true;
				KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
			}
			ImGui::EndPopup();
		}

		if (ImGui::Button("Volume", ImVec2(-1, 0)))
		{
			m_showVolume = true;
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);
	}

	ImGui::End();

	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar();
}

void TitleScene::DrawNormalButton()
{
	Math::Color color = { 1,1,1,m_buttonAlpha };
	if (m_playButtonTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_playButtonTex.get(), m_playButtonPos.x, m_playButtonPos.y, nullptr, &color);
	}
}
