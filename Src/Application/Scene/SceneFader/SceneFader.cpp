#include "SceneFader.h"
#include"../../main.h"
#include "Framework/KdFramework.h"

void SceneFader::Init()
{
	//m_blackTex = std::make_shared<KdTexture>();

	////1ピクセル分の色データを定義
	//unsigned char blackPixel[4] = { 0,0,0,255 };

	////初期化データを設定するための構造体を準備
	//D3D11_SUBRESOURCE_DATA subresourceData = {};
	////ピクセルデータへのポインタ設定(GPUにアップロードされる)
	//subresourceData.pSysMem = blackPixel;
	////1行あたりのデータサイズ（1ピクセルしかないので4バイト）
	//subresourceData.SysMemPitch = 4;

	////DXGI_FORMAT_R8G8B8A8_UNORMはもっとも標準的なRGBA形式
	//if (!m_blackTex->Create(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 1, &subresourceData))
	//{
	//	assert(0 && "黒テクスチャの生成に失敗しました");
	//}

	m_currentAlpha = 1.0f;
	m_state = FadeState::Idle;
}

void SceneFader::Update()
{
	float deltatime = Application::Instance().GetDeltaTime();

	if (m_state == FadeState::FadingOut)
	{
		m_currentAlpha += m_fadeSpeed * deltatime;
		if (m_currentAlpha >= 1.0f)
		{
			m_currentAlpha = 1.0f;
			m_state = FadeState::Idle;
		}
	}
	else if (m_state == FadeState::FadingIn)
	{
		m_currentAlpha -= m_fadeSpeed * deltatime;
		if (m_currentAlpha <= 0.0f)
		{
			m_currentAlpha = 0.0f;
			m_state = FadeState::Idle;
		}
	}
}

void SceneFader::Draw()
{
	if (!IsFadeing()) return;
	KdShaderManager::Instance().m_postProcessShader.DrawWipe(m_currentAlpha);
}
