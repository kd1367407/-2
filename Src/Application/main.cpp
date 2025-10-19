#include "main.h"
#include"../Application/Scene/SceneManager.h"
#include"../Application/Scene/BaseScene/BaseScene.h"
#include"../Framework/GameObject/ArchetypeManager.h"
#include"../Application/SettingsManager/SettingsManager.h"

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// エントリーポイント
// アプリケーションはこの関数から進行する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR , _In_ int)
{
	// メモリリークを知らせる
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
	{
		CoUninitialize();

		return 0;
	}

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//===================================================================
	// 実行
	//===================================================================
	Application::Instance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}

void Application::AddLog(const char* fmt, ...)
{
	// 可変長引数を扱うためのリスト
	va_list args;

	// fmt以降の引数をargsリストに格納
	va_start(args, fmt);

	// ★★★ KdDebugGUIのAddLogに、書式文字列と引数リストをそのまま渡す ★★★
	m_debugGui.AddLogV(fmt, args);

	// argsリストをクリーンアップ
	va_end(args);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginUpdate()
{
	// 入力状況の更新
	KdInputManager::Instance().Update();

	// ImGuiフレーム開始
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション更新終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostUpdate()
{
	// 3DSoundListnerの行列を更新
	KdAudioManager::Instance().SetListnerMatrix(KdShaderManager::Instance().GetCameraCB().mView.Invert());
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画開始
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdBeginDraw(bool usePostProcess)
{
	//バックバッファクリア
	KdDirect3D::Instance().ClearBackBuffer();

	//ポストプロセスを使う場合は、そのためのテクスチャもクリア
	if (!usePostProcess) return;
	KdShaderManager::Instance().m_postProcessShader.Draw();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション描画終了
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::KdPostDraw()
{
	//クリエイトモードのときだけデバッグGUIのロジックを呼び出す
	if (SceneManager::Instance().GetCurrentMode() == SceneManager::SceneMode::Create)
	{
#ifdef _DEBUG
		if (auto* scene = SceneManager::Instance().GetCurrentScene())
		{
			m_debugGui.GuiProcess(*scene);
		}
#endif
	}

	//ImGuiの描画コマンドを生成し、レンダリング
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	//ImGuiの描画より後に描画させる
	SceneManager::Instance().GetFader().Draw();

	// BackBuffer -> 画面表示
	KdDirect3D::Instance().WorkSwapChain()->Present(0, 0);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション初期設定
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
bool Application::Init(int w, int h)
{
	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "3D GameProgramming", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}

	//===================================================================
	// フルスクリーン確認
	//===================================================================
	bool bFullScreen = false;
//	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
//		bFullScreen = true;
//	}

	//===================================================================
	// Direct3D初期化
	//===================================================================

	// デバイスのデバッグモードを有効にする
	bool deviceDebugMode = false;
#ifdef _DEBUG
	deviceDebugMode = true;
#endif

	// Direct3D初期化
	std::string errorMsg;
	if (KdDirect3D::Instance().Init(m_window.GetWndHandle(), w, h, deviceDebugMode, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// フルスクリーン設定
	if (bFullScreen) {
		HRESULT hr;

		hr = KdDirect3D::Instance().SetFullscreenState(TRUE, 0);
		if (FAILED(hr))
		{
			MessageBoxA(m_window.GetWndHandle(), "フルスクリーン設定失敗", "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
			return false;
		}
	}


	//===================================================================
	// 各種マネージャー初期化
	//===================================================================
	SettingsManager::Instance().Init();
	ArchetypeManager::Instance().Init();
	InitInput();

	//===================================================================
	// シェーダー初期化
	//===================================================================
	KdShaderManager::Instance().Init();

	//===================================================================
	// オーディオ初期化
	//===================================================================
	KdAudioManager::Instance().Init();

	//===================================================================
	// imgui初期化
	//===================================================================
	m_debugGui.GuiInit();

	//シーンマネージャー初期化
	SceneManager::Instance().Init();

	return true;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// アプリケーション実行
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void Application::Execute()
{
	KdCSVData windowData("Asset/Data/WindowSettings.csv");
	const std::vector<std::string>& sizeData = windowData.GetLine(0);

	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (Application::Instance().Init(atoi(sizeData[0].c_str()), atoi(sizeData[1].c_str())) == false) {
		return;
	}

	//===================================================================
	// ゲームループ
	//===================================================================

	// 時間記録
	m_fpsController.Init();

	// ループ
	while (1)
	{
		//フレームの最初に呼び出してdeltatimeを計算、更新
		m_fpsController.Update();

		//計算済みのdeltatime取得
		m_deltaTime = m_fpsController.GetDeltaTime();

		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		if (GetAsyncKeyState(VK_ESCAPE))
		{
			bool shouldQuit = true;//デフォルト値

			if (auto scene = SceneManager::Instance().GetCurrentScene())
			{
				//シーンに未保存の変更があるか問い合わせる
				if (scene->HasUnsavedChanges())
				{
					if (MessageBoxA(m_window.GetWndHandle(), "保存されていない変更があります。\n本当に終了しますか？",
						"終了確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDNO)
					{
						shouldQuit = false;//終了をキャンセル
					}
				}
			}

			if (shouldQuit)
			{
				End();
			}
		}

		//=========================================
		//
		// アプリケーション更新処理
		//
		//=========================================

		KdBeginUpdate();
		{
			SceneManager::Instance().PreUpdate();

			SceneManager::Instance().Update();

			SceneManager::Instance().PostUpdate();
		}
		KdPostUpdate();

		//=========================================
		//
		// アプリケーション描画処理
		//
		//=========================================

		KdBeginDraw();
		{
			SceneManager::Instance().PreDraw();

			SceneManager::Instance().Draw();

			SceneManager::Instance().PostDraw();

			SceneManager::Instance().DrawSprite();
		}
		KdPostDraw();

		//=========================================
		//
		// フレームレート制御
		//
		//=========================================

		m_fpsController.Wait();

		std::string TitleBar = "Switch FPS:" + std::to_string(m_fpsController.m_nowfps);
		SetWindowTextA(m_window.GetWndHandle(), TitleBar.c_str());
	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}

// アプリケーション終了
void Application::Release()
{

	SceneManager::Instance().Release();

	m_debugGui.GuiRelease();

	KdInputManager::Instance().Release();

	KdShaderManager::Instance().Release();

	KdAudioManager::Instance().Release();

	KdDirect3D::Instance().Release();

	// ウィンドウ削除
	m_window.Release();
}

void Application::InitInput()
{
	const auto& inputSetting = SettingsManager::Instance().GetInputSetting();
	auto keyBordDeviceCollector = std::make_unique<KdInputCollector>();

	//文字列から仮想キーコードに変換するマップ
	const std::map<std::string, int> keyMap = {
		{"VK_F1", VK_F1},
		{"W", 'W'},
		{"S", 'S'},
		{"A", 'A'},
		{"D", 'D'},
		{"Z", 'Z'},
		{"VK_SPACE", VK_SPACE},
		{"VK_LBUTTON", VK_LBUTTON},
		{"VK_RBUTTON", VK_RBUTTON},
		{"VK_TAB",VK_TAB}
	};

	//キーボード設定をJsonから読み込み
	if (inputSetting.contains("keyboard"))
	{
		for (const auto& setting : inputSetting["keyboard"])
		{
			std::string name = setting.value("name", "");
			std::string key = setting.value("key", "");

			if (!name.empty() && keyMap.count(key))
			{
				keyBordDeviceCollector->AddButton(name, std::make_shared<KdInputButtonForWindows>(keyMap.at(key)));
			}
		}
	}

	//マウス設定をJsonから読み込み
	if (inputSetting.contains("mouse"))
	{
		for (const auto& setting : inputSetting["mouse"])
		{
			std::string name = setting.value("name", "");
			std::string button = setting.value("button", "");

			if (!name.empty() && keyMap.count(button))
			{
				keyBordDeviceCollector->AddButton(name, std::make_shared<KdInputButtonForWindows>(keyMap.at(button)));
			}
		}
	}

	KdInputManager::Instance().AddDevice("WindowsKeyBordDevice", keyBordDeviceCollector);
}
