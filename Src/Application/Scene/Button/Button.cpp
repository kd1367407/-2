#include "Button.h"
#include"../../main.h"
#include"../../UIHelpers/UIHelpers.h"

Button::Button(const Math::Vector2& pos, std::shared_ptr<KdTexture> tex, std::shared_ptr<DirectX::SpriteFont> font, const std::string& label, std::function<void()> onClickAction):
	m_pos(pos), m_tex(tex), m_spFont(font),m_label(label), m_onClick(onClickAction)
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

	//文字列
	if (m_spFont && !m_label.empty())
	{
		Math::Color textColor = { 1, 1, 1, m_alpha * baseAlpha };

		//文字列矩形計算
		DirectX::XMVECTOR labelSizeVec = m_spFont->MeasureString(m_label.c_str());

		float labelX = DirectX::XMVectorGetX(labelSizeVec);
		float labelY = DirectX::XMVectorGetY(labelSizeVec);

		//座標計算
		float textPosX = m_pos.x - (labelX / 2.0f);
		float textPosY = m_pos.y - (labelY / 2.0f);

		
	}
}
