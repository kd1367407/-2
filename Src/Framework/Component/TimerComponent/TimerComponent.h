#pragma once
#include"../Component.h"

class TransformComponent;

class TimerComponent:public Component
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;
	void DrawSprite()override;

	const float& GetElapsedTime()const { return m_elapsedTime; }
	void SetElapsedTime(float time) { m_elapsedTime = time; }
	const char* GetComponentName()const override { return "TimerComponent"; }

	bool UpdateCountUp(float deltatime);

	void StartCountUp(float targetTime);

private:
	std::shared_ptr<KdTexture> m_numTex;
	std::shared_ptr<KdTexture> m_colonTex;

	//経過時間
	float m_elapsedTime = 0.0f;

	float m_targetTime = 0.0f;
	float m_countUpSpeed = 1.0f;
	bool m_isCountUp = false;

	//各桁の数字描画
	void DrawNumber(int number, float x, float y);

	std::weak_ptr<TransformComponent> m_wpTransform;
};