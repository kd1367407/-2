#pragma once
#include"../BaseScene/BaseScene.h"

class TitleScene:public BaseScene
{
public:
	void Init()override;
	void SceneUpdate()override;
	void Draw()override;
	void DrawSprite()override;
	void Release()override;

private:
	void DrawTitleWindow();
	void DrawButtonWindow();
	void DrawNormalButton();

	float m_titleAlpha = 0.0f;
	float m_buttonAlpha = 0.0f;

	std::shared_ptr<KdTexture> m_titleTex;
	std::shared_ptr<KdTexture> m_playButtonTex;
	std::shared_ptr<KdTexture> m_createButtonTex;
	bool m_showTemplateSelect = false;//新規作成のテンプレート選択UIの状態
	bool m_showVolume = false;
};