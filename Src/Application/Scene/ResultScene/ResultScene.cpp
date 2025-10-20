#include "ResultScene.h"
#include"../GameScene/GameManager/GameManager.h"
#include"../Src/Framework/Component/TimerComponent/TimerComponent.h"
#include"../Src/Framework/Component/TransformComponent/TransformComponent.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/BackgroundComponent/BackgroundComponent.h"

void ResultScene::Init()
{
	//--データ取得--
	m_finalTime = GameManager::Instance().GetFinalTime();
	m_playerMoves = GameManager::Instance().GetFinalMoves();
	m_parMoves = GameManager::Instance().GetParMoves();
	
	//--TimerObject準備
	m_timerObject = std::make_shared<GameObject>();
	m_timerComp = m_timerObject->AddComponent<TimerComponent>();
	auto timerTransform = m_timerObject->AddComponent<TransformComponent>();
	m_timerPos = { -200.0f,0.0f };
	timerTransform->SetPos({ m_timerPos.x,m_timerPos.y,0.0f });
	timerTransform->SetScale({ 1.5f,1.5f,1.5f });
	m_timerObject->Init();
	AddObject(m_timerObject);
	
	//シーンの設定
	m_clearTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/UI/StageClear.png");
	m_playerMovesTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/UI/PlayerMoves1.png");
	m_parTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/UI/Par1.png");
	m_numTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Scene/Number-2.png");
	SceneManager::Instance().SetMode(SceneManager::SceneMode::UI);

	//--背景--
	auto backgroundObj = std::make_shared<GameObject>();
	backgroundObj->SetName("Background");
	backgroundObj->AddComponent<BackgroundComponent>();
	AddObject(backgroundObj);
	backgroundObj->Init();

	//--演出フラグ--
	m_showRank = false;
	m_hasCountUpStarted = false;

	//--サウンド-
	m_spBGM = KdAudioManager::Instance().Play("Asset/Sound/TitleBGM.wav", true, 1.0f);
}

void ResultScene::SceneUpdate()
{
	float deltatime = Application::Instance().GetDeltaTime();
	float fadeSpeed = 1.5f;
	auto& fader = SceneManager::Instance().GetFader();

	if (m_texAlpha < 1.0f)
	{
		if (!fader.IsFadeing())
		{
			m_texAlpha += fadeSpeed * deltatime;
		}
	}

	m_texAlpha = std::min(m_texAlpha, 1.0f);

	if (m_texAlpha >= 0.8f)
	{
		if (!m_hasCountUpStarted && !fader.IsFadeing())
		{
			//カウントアップ開始
			m_timerComp->StartCountUp(m_finalTime);
			m_hasCountUpStarted = true;
			//カウントアップ音
		}

		//カウントアップ終わったら
		if (!m_timerComp->UpdateCountUp(deltatime))
		{
			m_showRank = true;
		}
	}

	if (m_showRank)
	{
		m_uiAlpha += fadeSpeed * deltatime;
	}
	m_uiAlpha = std::min(m_uiAlpha, 1.0f);
}

void ResultScene::DrawSprite()
{
	auto& fader = SceneManager::Instance().GetFader();
	BaseScene::DrawSprite();

	if (!fader.IsFadeing())
	{
		if (m_timerObject)
		{
			m_timerObject->DrawSprite();
		}
	}

	DrawClearWindow();
	DrawMoveWindow();
	DrawRankWindow();
}

void ResultScene::Draw()
{
	BaseScene::Draw();
	DrawButtonWindow();
}

void ResultScene::Release()
{
	if (m_spBGM && m_spBGM->IsPlaying())
	{
		m_spBGM->Stop();
	}
	m_spBGM = nullptr;
}

void ResultScene::DrawClearWindow()
{
	Math::Color color = { 1,1,1,m_texAlpha };
	if (m_clearTex)
	{
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_clearTex.get(), m_timerPos.x+200, m_timerPos.y + 200, nullptr, &color);
		//KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerMovesTex.get(), m_timerPos.x + 200, m_timerPos.y + 200, nullptr, &color);
	}
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
		ImGui::SetNextWindowPos(ImVec2(viewport->GetCenter().x, viewport->WorkPos.y + 650), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
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

void ResultScene::DrawRankWindow()
{
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_uiAlpha);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->GetCenter().x, viewport->WorkPos.y + 500), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

	ImGui::Begin("Rank", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

	if (m_rankTex)
	{
		ImTextureID texID = m_rankTex->WorkSRView();
		ImVec2 texSize = ImVec2((float)m_rankTex->GetInfo().Width, (float)m_rankTex->GetInfo().Height);
		ImGui::Image(texID, texSize);
	}

	ImGui::End();

	ImGui::PopStyleColor(2);
	ImGui::PopStyleVar();
}

void ResultScene::DrawMoveWindow()
{
	Math::Color color = { 1,1,1,m_texAlpha };
	Math::Vector2 playerMovesPos = { m_timerPos.x + 130, m_timerPos.y - 100 };
	Math::Vector2 parPos = { playerMovesPos.x, playerMovesPos.y + 50 };
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_playerMovesTex.get(), playerMovesPos.x, playerMovesPos.y, nullptr, &color);
	DrawNumber(m_playerMoves, playerMovesPos.x + 140, playerMovesPos.y);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_parTex.get(), parPos.x, parPos.y, nullptr, &color);
	//DrawNumber(m_parMoves, parPos.x, parPos.y);
}

void ResultScene::DrawNumber(int number, float x, float y)
{
	if (number < 0 || number>9)return;

	const float	numTexWidth = 51.0f;
	const float	numTexHeight = 64.0f;

	//矩形計算
	Math::Rectangle srcRect;
	srcRect.x = numTexWidth * number;
	srcRect.y = 0;
	srcRect.width = numTexWidth;
	srcRect.height = numTexHeight;

	//描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_numTex, x, y, numTexWidth, numTexHeight, &srcRect);
}
