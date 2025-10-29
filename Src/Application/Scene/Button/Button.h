#pragma once
#include"Src/Framework/Utility/MyFont/MyFont.h"

class Button
{
public:
	Button(
		const Math::Vector2& pos,
		std::shared_ptr<KdTexture> tex,
		std::shared_ptr<MyFont> font,
		const std::string& label,
		std::function<void()> onClickAction
	);

	void Update();

	void Draw(float baseAlpha);

private:
	Math::Vector2 m_pos;
	Math::Vector2 m_size;
	std::shared_ptr<KdTexture> m_tex;
	Math::Rectangle m_rect;
	std::function<void()> m_onClick;//クリック時に実行する関数
	bool m_isOver;
	float m_alpha;
	std::shared_ptr<MyFont> m_spFont;
	std::string m_label;
};