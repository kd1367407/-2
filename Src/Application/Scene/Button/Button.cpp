#include "Button.h"
#include"../../main.h"

Button::Button(const Math::Vector2& pos, std::shared_ptr<KdTexture> tex, std::function<void()> onClickAction):
	m_pos(pos),m_tex(tex),m_onClick(onClickAction)
{
	if (m_tex)
	{
		m_size = {
			(float)m_tex->GetInfo().Width,
			(float)m_tex->GetInfo().Height
		};
	}
}

void Button::Update()
{
	float screenWidth = KdDirect3D::Instance().GetBackBuffer()->GetInfo().Width;
	float screenHeight = KdDirect3D::Instance().GetBackBuffer()->GetInfo().Height;

	m_rect = CalculateRectFromWorld(m_pos, m_size, screenWidth, screenHeight);

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

	m_isOver = m_rect.Contains(static_cast<long>(mousePos.x), static_cast<long>(mousePos.y));

	if (m_isOver)
	{
		m_alpha = 1.0f;
	}
	else
	{
		m_alpha = 0.8f;
	}

	if (m_isOver && KdInputManager::Instance().IsPress("Select"))
	{
		if (m_onClick)
		{
			m_onClick();
		}
	}
}

void Button::Draw(float baseAlpha)
{
	if (m_tex)
	{
		Math::Color color = { 1,1,1,m_alpha * baseAlpha };
		KdShaderManager::Instance().m_spriteShader.DrawTex(m_tex.get(), m_pos.x, m_pos.y, nullptr, &color);
	}
}

Math::Rectangle CalculateRectFromWorld(const Math::Vector2& vPos, const Math::Vector2& vSize, float screenWidth, float screenHeight)
{
	float centerScreenX = vPos.x + (screenWidth / 2.0f);
	float centerScreenY = (screenHeight / 2.0f) - vPos.y;

	long rectX = static_cast<long>(centerScreenX - (vSize.x / 2.0f));
	long rectY = static_cast<long>(centerScreenY - (vSize.y / 2.0f));

	return DirectX::SimpleMath::Rectangle(rectX, rectY, static_cast<long>(vSize.x), static_cast<long>(vSize.y));
}
