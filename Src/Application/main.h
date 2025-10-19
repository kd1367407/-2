#pragma once

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
// メンバ
public:

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End()							{ m_endFlag = true; }

	HWND GetWindowHandle()		const	{ return m_window.GetWndHandle(); }
	int GetMouseWheelValue()	const	{ return m_window.GetMouseWheelVal(); }

	int GetNowFPS()				const	{ return m_fpsController.m_nowfps; }
	int GetMaxFPS()				const	{ return m_fpsController.m_maxFps; }

	void AddLog(const char* fmt, ...);

	float GetDeltaTime()const { return m_deltaTime; }

private:

	void KdBeginUpdate();
	void KdPostUpdate();

	void KdBeginDraw(bool usePostProcess = true);
	void KdPostDraw();

	// アプリケーション初期化
	bool Init(int w, int h);

	// アプリケーション解放
	void Release();

	//KdInputManagerの初期化
	void InitInput();

	// ゲームウィンドウクラス
	KdWindow		m_window;

	// FPSコントローラー
	KdFPSController	m_fpsController;
	float m_deltaTime = 0.0f;

	// ImGUI機能
	KdDebugGUI		m_debugGui;

	// ゲーム終了フラグ trueで終了する
	bool		m_endFlag = false;

//=====================================================
// シングルトンパターン
//=====================================================
private:
	// 
	Application() {}

public:
	static Application &Instance(){
		static Application Instance;
		return Instance;
	}
};
