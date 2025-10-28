#pragma once

class Slider
{
public:
	Slider(const Math::Vector2& pos,
		std::shared_ptr<KdTexture> backTex,
		std::shared_ptr<KdTexture> knobTex,
		std::function<void(float)> onValueChanged);

	void Update();
	void Draw(float baseAlpha);

private:
	Math::Vector2 m_pos;
	Math::Vector2 m_size;//バーのサイズ
	Math::Rectangle m_rect;//バーの判定
	std::shared_ptr<KdTexture> m_backTex;
	std::shared_ptr<KdTexture> m_knobTex;
	float m_value;
	std::function<void(float)> m_onValueChanged;
};