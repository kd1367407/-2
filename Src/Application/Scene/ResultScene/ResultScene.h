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
	void DrawRankWindow();
	void DrawMoveWindow();
	void DrawNumber(int number, float x, float y);

	std::shared_ptr<GameObject> m_timerObject;
	std::shared_ptr<TimerComponent> m_timerComp;
	float m_finalTime = 0.0f;
	float m_uiAlpha = 0.0f;
	float m_texAlpha = 0.0f;
	Math::Vector2 m_timerPos = {};
	std::shared_ptr<KdTexture> m_clearTex;
	bool m_showRank = false;
	bool m_hasCountUpStarted = false;
	std::shared_ptr<KdTexture> m_rankTex;
	std::shared_ptr<KdTexture> m_playerMovesTex;
	std::shared_ptr<KdTexture> m_parTex;
	std::shared_ptr<KdTexture> m_numTex;
	int m_playerMoves;
	int m_parMoves;
};