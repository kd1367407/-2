#include "SceneManager.h"
#include"GameScene/GameScene.h"
#include"BaseScene/BaseScene.h"
#include"TitleScene/TitleScene.h"
#include"StageSelectScene/StageSelectScene.h"
#include"ResultScene/ResultScene.h"
#include"GameScene/GameManager/GameManager.h"

SceneManager& SceneManager::Instance()
{
	static SceneManager instance; 
	return instance;
}

void SceneManager::Init()
{
	//最初のシーン生成
	m_currentScene = std::make_unique<TitleScene>();
	m_currentScene->Init();

	m_fader.Init();
	m_fader.StartFadeIn();
}

void SceneManager::PreUpdate()
{
	// 空間環境の更新
	KdShaderManager::Instance().WorkAmbientController().Update();


	if (m_currentScene)
	{
		m_currentScene->PreUpdate();
	}
}

void SceneManager::Update()
{
	if (KdInputManager::Instance().IsPress("ModeChange"))
	{
		const auto& gm = GameManager::Instance();
		const auto& loadMode = gm.GetLoadMode();

		if (loadMode == GameManager::LoadMode::CreateNew || loadMode == GameManager::LoadMode::Edit)
		{
			ToggleMode();
		}
	}

	//毎フレーム、現在のモードに基づいてカーソル状態を強制する
	ImGuiIO& io = ImGui::GetIO();
	if (m_currentMode == SceneMode::Game)
	{
		//ImGuiにカーソルを描画しないでと伝える
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);
		//マウス入力をゲームモードで使えるようにImGuiによるキャプチャを無効化
		io.WantCaptureMouse = false;
	}

	if (m_currentScene)
	{
		m_currentScene->Update();
	}

	SceneFader::FadeState prevState = m_fader.GetState();
	m_fader.Update();

	//フェードアウトが完了したらシーン切り替え
	if (m_isChangingScene && prevState == SceneFader::FadeState::FadingOut && m_fader.GetState() == SceneFader::FadeState::Idle)
	{
		ExecuteSceneChange();
		m_isChangingScene = false;
	}
}

void SceneManager::PostUpdate()
{
	if (m_currentScene)
	{
		m_currentScene->PostUpdate();
	}
}

void SceneManager::PreDraw()
{
	//画面サイズをシェーダに送る
	KdShaderManager::Instance().WriteCBFrame();

	if (m_currentScene)
	{
		m_currentScene->PreDraw();
	}
}

void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->Draw();
	}
}

void SceneManager::PostDraw()
{
	// 画面のぼかしや被写界深度処理の実施
	KdShaderManager::Instance().m_postProcessShader.PostEffectProcess();

	if (m_currentScene)
	{
		m_currentScene->PostDraw();
	}
}

void SceneManager::DrawSprite()
{
	if (m_currentScene)
	{
		m_currentScene->DrawSprite();
	}
}

void SceneManager::Release()
{
	if (m_currentScene)
	{
		m_currentScene->Release();
		
		//管理しているオブジェクトを安全に破棄
		m_currentScene.reset();
	}
	
}

void SceneManager::ChangeScene(SceneType sceneType)
{
	if (m_isChangingScene)return;
	
	//次のシーンのインスタンスを予約
	switch (sceneType)
	{
	case SceneType::Title:
		m_nextScene = std::make_unique<TitleScene>();
		break;
	case SceneType::StageSelect:
		m_nextScene = std::make_unique<StageSelectScene>();
		break;
	case SceneType::Game:
		m_nextScene = std::make_unique<GameScene>();
		break;
	case SceneType::Result:
		m_nextScene = std::make_unique<ResultScene>();
		break;
	}
	m_isChangingScene = true;
	m_fader.StartFadeOut();
}

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

void SceneManager::ToggleMode()
{
	m_currentMode = (m_currentMode == SceneMode::Create) ? SceneMode::Game : SceneMode::Create;

	//現在のシーンにモードが変更されたことを通知
	if (m_currentScene)
	{
		m_currentScene->OnModeChanged(m_currentMode);
	}
}

void SceneManager::ExecuteSceneChange()
{
	//現在のシーンがあれば解放
	if (m_currentScene)
	{
		m_currentScene->Release();
	}

	//現在のシーンを次のシーンに差し替える
	m_currentScene = std::move(m_nextScene);

	//新しいシーンの初期化処理
	if (m_currentScene)
	{
		m_currentScene->Init();
		m_fader.StartFadeIn();
	}
}
