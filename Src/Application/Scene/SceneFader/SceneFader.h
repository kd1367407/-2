#pragma once

class SceneFader
{
public:
	enum class FadeState
	{
		Idle,
		FadingOut,//だんだん暗く
		FadingIn//だんだん明るく
	};

	void Init();
	void Update();
	void Draw();

	void StartFadeOut() 
	{
		m_state = FadeState::FadingOut; 
		m_currentAlpha = 0.0f;
		KdAudioManager::Instance().Play("Asset/Sound/SceneFade.wav", false, 1.0f);
	}
	void StartFadeIn() 
	{
		m_state = FadeState::FadingIn; 
		m_currentAlpha = 1.0f;
		KdAudioManager::Instance().Play("Asset/Sound/SceneFade.wav", false, 1.0f);
	}

	bool IsFadeing()const { return m_state != FadeState::Idle; }
	FadeState GetState()const { return m_state; }
	float GetCurrentAlpha()const { return m_currentAlpha; }

private:
	FadeState m_state = FadeState::Idle;
	float m_currentAlpha = 0.0f;
	float m_fadeSpeed = 0.5f;

	//std::shared_ptr<KdTexture> m_blackTex;
};