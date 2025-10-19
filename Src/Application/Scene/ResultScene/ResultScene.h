#pragma once
#include"../BaseScene/BaseScene.h"

class GameObject;

class ResultScene :public BaseScene
{
public:
	void Init()override;
	void SceneUpdate()override;
	void DrawSprite()override;
	void Draw()override;

private:
	void DrawClearWindow();
	void DrawButtonWindow();

	std::shared_ptr<GameObject> m_timerObject;
	float m_finalTime = 0.0f;
	float m_uiAlpha = 0.0f;
	float m_texAlpha = 0.0f;
	Math::Vector2 m_timerPos = {};
	std::shared_ptr<KdTexture> m_clearTex;
};