#include "KdFPSController.h"

// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
// FPSの制御コントローラー
// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
void KdFPSController::Init()
{
	//Init時に現在の時間を記録しておく
	m_frameStartTime = timeGetTime();
	m_fpsMonitorBeginTime = timeGetTime();
}

void KdFPSController::Update()
{
	//前回のフレーム開始時刻からの差を計算し、deltatimeとする
	DWORD currentTime = timeGetTime();
	float diff = static_cast<float>(currentTime - m_frameStartTime);
	m_deltaTime = diff / static_cast<float>(kSecond);

	//deltatimeが大きくなりすぎないように制限
	if (m_deltaTime > MaxDelta)
	{
		m_deltaTime = MaxDelta;
	}

	//現在の時刻を今回のフレームの開始時間として記録
	m_frameStartTime = currentTime;

	Monitoring();
}

void KdFPSController::Wait()
{
	//処理終了時間取得
	DWORD frameProcessEndTime = timeGetTime();

	//1フレームに許容される時間
	DWORD timePerFrame = kSecond / m_maxFps;

	//実際にかかった1フレームの時間
	DWORD processTime = frameProcessEndTime - m_frameStartTime;

	if (processTime < timePerFrame)
	{
		//1秒間にMaxFPS回数以上処理が回らないように待機する
		Sleep(timePerFrame - processTime);
	}
}

// 現在のFPS計測
void KdFPSController::Monitoring()
{
	// FPS計測のタイミング　0.5秒おき
	constexpr float kFpsRefreshFrame = 500;		

	m_fpsCnt++;

	// 0.5秒おきに FPS計測
	if (m_frameStartTime - m_fpsMonitorBeginTime >= kFpsRefreshFrame)
	{
		// 現在のFPS算出
		m_nowfps = (m_fpsCnt * kSecond) / (m_frameStartTime - m_fpsMonitorBeginTime);

		m_fpsMonitorBeginTime = m_frameStartTime;

		m_fpsCnt = 0;
	}
}
