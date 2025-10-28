#pragma once
#include"../BaseScene/BaseScene.h"
#include"../Button/Button.h"

class TitleScene:public BaseScene
{
public:
	enum class MenuState
	{
		Main,
		Create,
		NewStage,
		Volume
	};

	void Init()override;
	void SceneUpdate()override;
	void Draw()override;
	void DrawSprite()override;
	void Release()override;

private:
	void DrawTitleWindow();
	void DrawButtonWindow();

	float m_titleAlpha = 0.0f;
	float m_buttonAlpha = 0.0f;
	Math::Vector2 m_playButtonPos = {};

	std::shared_ptr<KdTexture> m_titleTex;
	std::shared_ptr<KdTexture> m_playButtonTex;
	std::shared_ptr<KdTexture> m_createButtonTex;
	std::shared_ptr<KdTexture> m_editButtonTex;
	std::shared_ptr<KdTexture> m_newButtonTex;
	std::shared_ptr<KdTexture> m_foundation1ButtonTex;
	std::shared_ptr<KdTexture> m_foundation2ButtonTex;
	std::shared_ptr<KdTexture> m_foundation3ButtonTex;
	std::shared_ptr<KdTexture> m_backButtonTex;
	bool m_showTemplateSelect = false;//新規作成のテンプレート選択UIの状態
	bool m_showVolume = false;

	std::map<MenuState, std::vector<Button>> m_buttons;
	MenuState m_currentState;
};