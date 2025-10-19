#pragma once
#include"../BaseScene/BaseScene.h"

class GameObject;
class TimerComponent;

class ResultScene :public BaseScene
{
public:
	void Init()override;
	void SceneUpdate()override;
	void DrawSprite()override;
	void Draw()override;
	void Release()override;

private:
	void DrawClearWindow();
	void DrawButtonWindow();

	std::shared_ptr<GameObject> m_timerObject;
	std::shared_ptr<TimerComponent> m_timerComp;
	float m_finalTime = 0.0f;
	float m_uiAlpha = 0.0f;
	float m_texAlpha = 0.0f;
	Math::Vector2 m_timerPos = {};
	std::shared_ptr<KdTexture> m_clearTex;
	bool m_isCountingUp = true;
	bool m_showRank = false;
	bool m_hasCountUpStarted = false;
};