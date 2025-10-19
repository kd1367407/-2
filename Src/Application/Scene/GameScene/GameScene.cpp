#include "GameScene.h"
#include"../../GameViewModel.h"
#include"../Src/Framework/Input/InputHandler/InputHandler.h"
#include"../Src/Framework/Command/CommandInvoker/CommandInvoker.h"
#include"../../GameLogic/StageModel/StageModel.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/CameraComponent/CameraComponent.h"
#include"../Src/Framework/Component/CameraComponent/EditorCameraComponent/EditorCameraComponent.h"
#include"../Src/Framework/Component/SolutionVisualizerComponent/SolutionVisualizerComponent.h"
#include"../Src/Framework/Component/TagComponent/TagComponent.h"
#include"../../System/PhysicsSystem.h"
#include"../SceneManager.h"
#include"../GameScene/GameManager/GameManager.h"
#include"../../StageListManager/StageListManager.h"
#include"../../main.h"
using json = nlohmann::json;

void GameScene::Init()
{
	m_bShowControlsWindow = false;
	//1:主要なシステムとModelを生成
	m_gameInputHandler = std::make_shared<InputHandler>();
	m_commandInvoker = std::make_shared<CommandInvoker>();
	m_spStageModel = std::make_shared<StageModel>();
	m_spStageModel->Init();
	m_debugWire = std::make_shared<KdDebugWireFrame>();

	//2:ViewModelを生成してModelとInvokerを渡す
	m_spGameViewModel = std::make_shared<GameViewModel>(m_spStageModel, m_commandInvoker, this);

	//3:InputHandlerの報告先をViewModelに設定
	m_gameInputHandler->SetViewModel(m_spGameViewModel);

	//5:ViewModelにステージ読み込みを指示
	const std::string& stageToLoad = GameManager::Instance().GetCurrentFilePath();
	m_spGameViewModel->LoadStage(stageToLoad);

	//6:Editor初期化
	m_editor.Init();

	auto visualizerObj = std::make_shared<GameObject>();
	visualizerObj->SetName("SolutionVisualizer");
	visualizerObj->AddComponent<TagComponent>();
	auto visComp = visualizerObj->AddComponent<SolutionVisualizerComponent>();
	visComp->SetViewModel(m_spGameViewModel);
	AddObject(visualizerObj);
	visualizerObj->Init();
	GameManager::Instance().StartNewGame();
	GameManager::Instance().SetParMoves(m_spGameViewModel->GetLoadedParMoves());

	m_particleSystem.Init(this);

	//7:現在のモードに応じてアクティブカメラを設定
	OnModeChanged(SceneManager::Instance().GetCurrentMode());

	m_spBGM = KdAudioManager::Instance().Play("Asset/Sound/GameBGM.wav", true, 1.0f);
}

void GameScene::SceneUpdate()
{

	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Game|| SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::UI)
	{
		m_gameInputHandler->Update();

		m_particleSystem.Update(Application::Instance().GetDeltaTime());

		if (KdInputManager::Instance().IsPress("Tab"))
		{
			m_bShowControlsWindow = !m_bShowControlsWindow;

			if (m_bShowControlsWindow)
			{
				SceneManager::Instance().SetMode(SceneManager::SceneMode::UI);
			}
			else
			{
				SceneManager::Instance().SetMode(SceneManager::SceneMode::Game);
			}
		}
	}
	else if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{
		//アンドゥ処理
		if (KdInputManager::Instance().IsPress("Undo"))
		{
			m_commandInvoker->UndoLastCommand();
		}
	}
}

void GameScene::PostUpdate()
{
	BaseScene::PostUpdate();
}

void GameScene::Draw()
{
	//現在のモードによって処理を分岐
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{
		//描画先を画面からエディタのレンダーターゲットに変更
		KdRenderTargetChanger rtChanger;
		rtChanger.ChangeRenderTarget(m_editor.GetSceneRT());

		//レンダーターゲットをクリア
		m_editor.GetSceneRT().ClearTexture();

		//全ての3Dオブジェクトをレンダーターゲットに描画
		BaseScene::Draw();

		//描画先を元に戻す(rtChangerのデストラクタで自動で戻る)

		//エディタUI描画
		m_editor.Draw(*this);
		if (m_debugWire)
		{
			PhysicsSystem::Instance().DrawDebug(*m_debugWire);
			m_debugWire->Draw();
		}
	}
	else
	{
		BaseScene::Draw();

		m_particleSystem.Draw();

		if (m_bShowControlsWindow)
		{
			DrawControlsWindow();
		}

		if (m_bShowTutorialHint)
		{
			DrawTutorialHintWindow();
		}
	}
}

void GameScene::Release()
{
	m_objList.clear();
	if (m_spBGM && m_spBGM->IsPlaying())
	{
		m_spBGM->Stop();
	}
	m_spBGM = nullptr;
}

void GameScene::OnModeChanged(SceneManager::SceneMode newMode)
{
	if (newMode == SceneManager::SceneMode::Create)
	{
		if (auto editorCam = FindObject("EditorCamera"))
		{
			SetActiveCamera(editorCam->GetComponent<EditorCameraComponent>());
		}
	}
	else
	{
		if (auto tpsCam = FindObject("TPS_Camera"))
		{
			SetActiveCamera(tpsCam->GetComponent<CameraComponent>());
		}
	}
}

bool GameScene::HasUnsavedChanges() const
{
	if (m_spGameViewModel)
	{
		return m_spGameViewModel->IsDirty();
	}
	return false;
}

void GameScene::RequestSaveStage()
{
	if (!m_spGameViewModel)return;

	auto& gm = GameManager::Instance();

	//新規作成での初回保存か
	if (gm.GetLoadMode() == GameManager::LoadMode::CreateNew)
	{
		auto& slm = StageListManager::Instance();

		//新しいファイルパスを生成
		std::string newFilePath = slm.GenerateNewStagePath();

		//ViewModelに保存を命令
		m_spGameViewModel->SaveStage(newFilePath);

		//StageListManagerに新しいエントリを追加
		const std::string& newLabel = gm.GetCurrentStageLabel();
		slm.AddStageEntry(newLabel, newFilePath);

		//GameManagerの更新
		gm.UpdateAfterNewStageSave(newFilePath);

		Application::Instance().AddLog("New stage saved and added to StageList.");
	}
	else//既存のステージの保存
	{
		m_spGameViewModel->SaveStage(gm.GetCurrentFilePath());
		UpdateStageListLabel();
	}
}

void GameScene::Undo()
{
	if (m_commandInvoker)
	{
		m_commandInvoker->UndoLastCommand();
	}
}

void GameScene::UndoClearFrimEditor()
{
	if (m_commandInvoker)
	{
		m_commandInvoker->Clear();
	}
}

void GameScene::UpdateStageListLabel()
{
	const auto& gm = GameManager::Instance();
	const std::string& filePath = gm.GetCurrentFilePath();
	const std::string& label = gm.GetCurrentStageLabel();

	StageListManager::Instance().UpdateStageLabel(filePath, label);
}

std::shared_ptr<EditorCameraComponent> GameScene::GetEditorCamera() const
{
	if (auto editorCam = FindObject("EditorCamera"))
	{
		return editorCam->GetComponent<EditorCameraComponent>();
	}
	return nullptr;
}

void GameScene::ShowTutorialHint(const std::string& text, const std::string& imagePath, const std::string& blockName)
{
	if (m_bShowTutorialHint)return;

	//一応
	m_blockName.clear();
	m_hitText.clear();
	m_pHintTexture = nullptr;

	m_bShowTutorialHint = true;
	m_blockName = blockName;
	m_hitText = text;
	if (!imagePath.empty())
	{
		m_pHintTexture = KdAssets::Instance().m_textures.GetData(imagePath);
	}
	SceneManager::Instance().SetMode(SceneManager::SceneMode::UI);
}

void GameScene::DrawControlsWindow()
{
	//ウィンドウの角の丸みを大きくする
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 20.0f);

	//ウィンドウ内の余白を広げる
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));

	//タイトルバー(アクティブ)の色指定
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

	//ウィンドウの背景色の色指定
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

	//テキストの色指定
	//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.8f, 1.0f));

	ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_Appearing);

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::Begin(U8("操作方法"), &m_bShowControlsWindow))
	{
		ImGui::Text(U8("=== 基本操作 ==="));
		ImGui::BulletText(U8("W, A, S, D : 移動"));
		ImGui::BulletText(U8("スペースキー : ジャンプ"));
		ImGui::BulletText(U8("マウス : 視点移動"));

		ImGui::Separator();

		ImGui::Text(U8("=== ブロック操作 ==="));
		ImGui::BulletText(U8("左クリック : ブロックを選択"));
		ImGui::TextWrapped(U8("2つのブロックを選択すると、\nそれらの位置が入れ替わります。"));

		ImGui::Separator();

		if (ImGui::Button(U8("ステージセレクト"), ImVec2(120, 0)))
		{
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::StageSelect);
		}

		ImGui::Spacing();

		if (ImGui::Button(U8("タイトル"), ImVec2(120, 0)))
		{
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Title);
		}

		ImGui::Spacing();

		if (ImGui::Button(U8("閉じる"), ImVec2(120, 0)))
		{
			m_bShowControlsWindow = false;
			SceneManager::Instance().SetMode(SceneManager::SceneMode::Game);
		}
	}
	ImGui::End();

	ImGui::PopStyleColor(2);

	ImGui::PopStyleVar(2);
}

void GameScene::DrawTutorialHintWindow()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	
	//画面を覆うオーバーレイ(スクリム)描画
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("TutorialBackground", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);//タイトルバーなし、操作不能
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);//角を丸く
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));//余白を広めに
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);//枠線を消す

	//コンテンツパネル描画
	ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(450, 0));

	ImGui::Begin("Tutorial Panel", &m_bShowTutorialHint, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	//文字サイズ変更
	ImGui::SetWindowFontScale(1.2f);

	//テクスチャ表示
	if (m_pHintTexture)
	{
		//中央に配置
		float imageWidth = m_pHintTexture->GetInfo().Width;
		ImGui::SetCursorPosX((ImGui::GetWindowWidth() - imageWidth) * 0.5f);

		//ID3D11ShaderResourceView* を取得
		ImTextureID texId = m_pHintTexture->WorkSRView();

		//画像サイズ取得
		ImVec2 texSize = ImVec2(imageWidth, m_pHintTexture->GetInfo().Height);

		ImGui::Image(texId, texSize);
	}

	ImGui::Spacing();

	//ブロック名
	if (!m_blockName.empty())
	{
		ImGui::SeparatorText(m_blockName.c_str());
		ImGui::Spacing();
	}

	//テキスト表示
	if (!m_hitText.empty())
	{
		ImGui::TextWrapped(U8("%s"), m_hitText.c_str());
		ImGui::Separator();
	}

	float buttonWidth = 120;
	ImGui::SetCursorPosX((ImGui::GetWindowWidth() - buttonWidth) * 0.5f);
	if (ImGui::Button("OK", ImVec2(buttonWidth, 0)))
	{
		m_bShowTutorialHint = false;
		SceneManager::Instance().SetMode(SceneManager::SceneMode::Game);
	}

	//'X'ボタン
	if (!m_bShowTutorialHint)
	{
		SceneManager::Instance().SetMode(SceneManager::SceneMode::Game);
	}

	ImGui::End();

	ImGui::PopStyleVar(3);
}
