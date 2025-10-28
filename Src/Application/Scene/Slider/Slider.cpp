#include "Slider.h"
#include"../../main.h"

Slider::Slider(const Math::Vector2& pos, std::shared_ptr<KdTexture> backTex, std::shared_ptr<KdTexture> knobTex, std::function<void(float)> onValueChanged):
	m_pos(pos),m_backTex(backTex),m_knobTex(knobTex),m_onValueChanged(onValueChanged)
{
	if (m_backTex)
	{
		m_size = { 
			(float)m_backTex->GetInfo().Width,
			(float)m_backTex->GetInfo().Height
		};
	}
}

void Slider::Update()
{
	float screenWidth = KdDirect3D::Instance().GetBackBuffer()->GetInfo().Width;
	float screenHeight = KdDirect3D::Instance().GetBackBuffer()->GetInfo().Height;

	m_rect = CalculateRectFromWorld(m_pos, m_size, screenWidth, screenHeight);

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

	bool isOver = m_rect.Contains(static_cast<long>(mousePos.x), static_cast<long>(mousePos.y));

	if (isOver && KdInputManager::Instance().IsHold("Select"))
	{
		//マウスのX座標をスライダーRectからの相対座標へ
		float mouseRelX = static_cast<float>(mousePos.x - m_rect.x);
		float newValue = mouseRelX / m_rect.width;

		newValue = std::max(0.0f, std::min(1.0f, newValue));

		if (m_value != newValue)
		{
			m_value = newValue;

			if (m_onValueChanged)
			{
				m_onValueChanged(m_value);
			}
		}
	}
}

void Slider::Draw(float baseAlpha)
{
}
