#pragma once

class MyFont
{
public:
	MyFont() = default;
	~MyFont() { Release(); }

	bool Load(std::string_view fileName);

	void Release() { m_spFont.reset(); }

	DirectX::SpriteFont* GetFont() const { return m_spFont.get(); }

private:
	std::unique_ptr<DirectX::SpriteFont> m_spFont = nullptr;
};