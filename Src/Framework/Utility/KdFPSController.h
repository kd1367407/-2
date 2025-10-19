#pragma once

//============================================================
// アプリケーションのFPS制御 + 測定
//============================================================
struct KdFPSController
{
	// FPS制御
	int		m_nowfps = 0;		// 現在のFPS値
	int		m_maxFps = 60;		// 最大FPS

	void Init();

	//deltatime計算とFPS監視に特化
	void Update();

	//待機処理
	void Wait();

	float GetDeltaTime()const { return m_deltaTime; }

private:

	void Monitoring();

	DWORD		m_frameStartTime = 0;		// フレームの開始時間

	int			m_fpsCnt = 0;				// FPS計測用カウント
	DWORD		m_fpsMonitorBeginTime = 0;	// FPS計測開始時間

	const int	kSecond = 1000;				// １秒のミリ秒

	const float MaxDelta = 0.1f;

	float m_deltaTime = 0.0f;//デルタタイム
};
