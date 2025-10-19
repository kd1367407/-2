#include "BackgroundComponent.h"
#include"../Src/Application/main.h"

void BackgroundComponent::Awake()
{
	m_normalTex1 = KdAssets::Instance().m_textures.GetData("Asset/Textures/grid4.png");
	m_normalTex2 = KdAssets::Instance().m_textures.GetData("Asset/Textures/grid3.png");
	/*m_normalTex1 = KdAssets::Instance().m_textures.GetData("Asset/Textures/grid3.png");
	m_normalTex2 = KdAssets::Instance().m_textures.GetData("Asset/Textures/grid1.png");*/
}

void BackgroundComponent::Start()
{
}

void BackgroundComponent::Update()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	m_uvOffset1.x += 0.02f * deltaTime;
	m_uvOffset2.y += 0.005f * deltaTime;

	if (m_uvOffset1.x > 0.5f) m_uvOffset1.x -= 0.5f;
	if (m_uvOffset2.y > 0.5f) m_uvOffset2.y -= 0.5f;
}

void BackgroundComponent::DrawSprite()
{
	auto& spriteShader = KdShaderManager::Instance().m_spriteShader;

	//グリッド描画有効化
	spriteShader.SetGridEnable(true);

	//offset転送
	spriteShader.SetGridUVOffset(m_uvOffset1, m_uvOffset2);

	//タイリング
	spriteShader.SetUVTiling({ 3,3 });

	//画像転送
	KdShaderManager::Instance().m_spriteShader.SetGridNomalTexture(*m_normalTex1, *m_normalTex2);

	KdShaderManager::Instance().ChangeSamplerState(KdSamplerState::Point_Wrap);

	spriteShader.DrawTex(
		m_normalTex1.get(), 0, 0,
		(int)KdDirect3D::Instance().GetBackBuffer()->GetWidth(),
		(int)KdDirect3D::Instance().GetBackBuffer()->GetHeight()
	);

	KdShaderManager::Instance().UndoSamplerState();

	spriteShader.SetUVTiling({ 1,1 });

	spriteShader.SetGridEnable(false);
}
