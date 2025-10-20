#include "TimerComponent.h"
#include"../Src/Application/main.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../Src/Application/Scene/GameScene/GameManager/GameManager.h"

void TimerComponent::Awake()
{
	m_numTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Scene/Number-2.png");
	m_colonTex = KdAssets::Instance().m_textures.GetData("Asset/Textures/Scene/colon1.png");
}

void TimerComponent::Start()
{
	if (m_owner)
	{
		m_wpTransform = m_owner->GetComponent<TransformComponent>();
	}
}

void TimerComponent::Update()
{
	if (SceneManager::Instance().GetCurrentMode() != SceneManager::SceneMode::Game)
	{
		return;
	}
	
	float deltaTime = Application::Instance().GetDeltaTime();
	auto& fader = SceneManager::Instance().GetFader();
	
	if (!fader.IsFadeing())
	{
		if (m_isTimerActive)
		{
			m_elapsedTime += deltaTime;
		}
	}
}

void TimerComponent::DrawSprite()
{
	if (GameManager::Instance().GetLoadMode() != GameManager::LoadMode::Play)return;

	if (!m_numTex || !m_colonTex)return;

	//Transformから基準座標取得
	Math::Vector3 basePos = Math::Vector3::Zero;
	if (auto spTransform = m_wpTransform.lock())
	{
		basePos = spTransform->GetPos();
	}

	//経過時間を分/秒/ミリ秒に分解
	int totalSeconds = static_cast<int>(m_elapsedTime);
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	int milliseconds = static_cast<int>((m_elapsedTime - totalSeconds) * 100);

	//各桁描画
	DrawNumber(minutes / 10, basePos.x, basePos.y);
	DrawNumber(minutes % 10, basePos.x + 51, basePos.y);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_colonTex, basePos.x + 102, basePos.y);
	DrawNumber(seconds / 10, basePos.x + 153, basePos.y);
	DrawNumber(seconds % 10, basePos.x + 204, basePos.y);
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_colonTex, basePos.x + 255, basePos.y);
	DrawNumber(milliseconds / 10, basePos.x + 306, basePos.y);
	DrawNumber(milliseconds % 10, basePos.x + 357, basePos.y);
}

bool TimerComponent::UpdateCountUp(float deltatime)
{
	if (!m_isCountUp)return false;

	m_elapsedTime += m_countUpSpeed * deltatime;

	if (m_elapsedTime >= m_targetTime)
	{
		m_elapsedTime = m_targetTime;
		m_isCountUp = false;
		return false;//カウントアップ完了
	}
	return true;
}

void TimerComponent::StartCountUp(float targetTime)
{
	m_targetTime = targetTime;
	m_elapsedTime = 0.0f;
	m_isCountUp = true;

	if (m_targetTime > 0.0f)
	{
		float animationDirection = 1.5f;
		m_countUpSpeed = m_targetTime / animationDirection;
	}
	else
	{
		m_countUpSpeed = 1.0f;
	}
}

void TimerComponent::DrawNumber(int number, float x, float y)
{
	if (number < 0 || number>9)return;

	const float	numTexWidth = 51.0f;
	const float	numTexHeight = 64.0f;

	//矩形計算
	Math::Rectangle srcRect;
	srcRect.x = numTexWidth * number;
	srcRect.y = 0;
	srcRect.width = numTexWidth;
	srcRect.height = numTexHeight;

	//描画
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_numTex, x, y, numTexWidth, numTexHeight, &srcRect);
}
