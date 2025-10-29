#include "TitleScene.h"
#include"../SceneManager.h"
#include"../GameScene/GameManager/GameManager.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/BackgroundComponent/BackgroundComponent.h"
#include"../../main.h"
#include"../../SettingsManager/SettingsManager.h"
#include"../Button/Button.h"

void TitleScene::Init()
{
	SceneManager::Instance().SetMode(SceneManager::SceneMode::UI);
	m_showTemplateSelect = false;

	m_titleTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/UI/Title2.png");
	m_playButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/PlayButtonKari.png");
	m_createButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/CreateButtonKari.png");
	m_editButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/CreateExistingStageKari.png");
	m_newButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/CreateNewStage.png");
	m_foundation1ButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Template1Kari.png");
	m_foundation2ButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Template2Kari.png");
	m_foundation3ButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Template3Kari.png");
	m_backButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/BackButtonKari.png");
	m_volumeButtonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/VolumeButtonKari.png");
	m_BGMSliderTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/BGMsliderKari.png");
	m_SESliderTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/SEsliderKari.png");
	m_knobTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/knobKari.png");

	//フォントロード
	auto spFont = KdAssets::Instance().m_fonts.GetData("Asset/Font/NotoSansJP-Regular.ttf");

	//背景
	auto backgroundObj = std::make_shared<GameObject>();
	backgroundObj->SetName("Background");
	backgroundObj->AddComponent<BackgroundComponent>();
	AddObject(backgroundObj);
	backgroundObj->Init();

	m_spBGM = KdAudioManager::Instance().Play("Asset/Sound/TitleBGM.wav", true, 1.0f);

	//--通常メニュー--
	m_buttons[MenuState::Main].emplace_back(Math::Vector2(0, 0), m_playButtonTex, spFont, "",
		[this]() {
			GameManager::Instance().SetLoadMode(GameManager::LoadMode::Play);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::StageSelect);
		}
	);

	m_buttons[MenuState::Main].emplace_back(Math::Vector2(0, -100), m_createButtonTex, spFont, "",
		[this]() {
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
			m_currentState = MenuState::Create;
		}
	);

	m_buttons[MenuState::Main].emplace_back(Math::Vector2(0, -200), m_volumeButtonTex, spFont, "",
		[this]() {
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
			m_currentState = MenuState::Volume;
		}
	);

	//--ステージ作成メニュー--
	m_buttons[MenuState::Create].emplace_back(Math::Vector2(0, 0), m_editButtonTex, spFont, "",
		[this]() {
			GameManager::Instance().SetLoadMode(GameManager::LoadMode::Edit);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::StageSelect);
		}
	);

	m_buttons[MenuState::Create].emplace_back(Math::Vector2(0, -100), m_newButtonTex, spFont, "",
		[this]() {
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
			m_currentState = MenuState::NewStage;
		}
	);

	//--ステージ土台メニュー--
	m_buttons[MenuState::NewStage].emplace_back(Math::Vector2(0, 0), m_foundation1ButtonTex, spFont, "",
		[this]() {
			auto& gm = GameManager::Instance();
			gm.SetLoadMode(GameManager::LoadMode::CreateNew);
			gm.SetNextStage("Asset/Data/Stages/Template01.json", "New Stage(Foundation 1)");
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Create);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
		}
	);

	m_buttons[MenuState::NewStage].emplace_back(Math::Vector2(0, -100), m_foundation2ButtonTex, spFont, "",
		[this]() {
			auto& gm = GameManager::Instance();
			gm.SetLoadMode(GameManager::LoadMode::CreateNew);
			gm.SetNextStage("Asset/Data/Stages/Template02.json", "New Stage(Foundation 2)");
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Create);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
		}
	);

	m_buttons[MenuState::NewStage].emplace_back(Math::Vector2(0, -200), m_foundation3ButtonTex, spFont, "",
		[this]() {
			auto& gm = GameManager::Instance();
			gm.SetLoadMode(GameManager::LoadMode::CreateNew);
			gm.SetNextStage("Asset/Data/Stages/Template03.json", "New Stage(Foundation 3)");
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Create);
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
		}
	);

	m_buttons[MenuState::NewStage].emplace_back(Math::Vector2(0, -300), m_backButtonTex, spFont, "",
		[this]() {
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
			m_currentState = MenuState::Main;
		}
	);

	//--sound--
	m_sliders[MenuState::Volume].emplace_back(Math::Vector2(0, 0), m_BGMSliderTex, m_knobTex,
		[this](float newVolume) {
			KdAudioManager::Instance().SetBGMVolume(newVolume);
		}
	);

	m_sliders[MenuState::Volume].emplace_back(Math::Vector2(0, -100), m_SESliderTex, m_knobTex,
		[this](float newVolume) {
			KdAudioManager::Instance().SetSEVolume(newVolume);
		}
	);

	m_buttons[MenuState::Volume].emplace_back(Math::Vector2(0, -300), m_backButtonTex, spFont, "",
		[this]() {
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);

			//書き込み可能なjsonオブジェクトを取得
			nlohmann::json& settings = SettingsManager::Instance().WorkGameSetting();

			//"volume_settings"の値を上書き
			settings["volume_settings"]["SE"] = KdAudioManager::Instance().GetSEVolume();
			settings["volume_settings"]["BGM"] = KdAudioManager::Instance().GetBGMVolume();

			//save指示(変更したjsonをファイルに書き出し)
			SettingsManager::Instance().SaveGameSetting();

			m_currentState = MenuState::Main;
		}
	);

	m_currentState = MenuState::Main;
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

	for (auto& button : m_buttons[m_currentState])
	{
		button.Update();
	}

	if (m_currentState == MenuState::Volume)
	{
		for (auto& slider : m_sliders[m_currentState])
		{
			slider.Update();
		}
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

	for (auto& button : m_buttons[m_currentState])
	{
		button.Draw(m_buttonAlpha);
	}

	if (m_currentState == MenuState::Volume)
	{
		for (auto& slider : m_sliders[m_currentState])
		{
			slider.Draw(1.0f);
		}
	}
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
