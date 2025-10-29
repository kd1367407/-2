#pragma once
#include"../Inc/SpriteFont.h"

class MyFont
{
public:
	MyFont() = default;
	~MyFont() { Release(); }

	bool Load(std::string_view fileName);

	DirectX::XMVECTOR MeasureString(const char* text) const;

	void Release() { m_spFont.reset(); }

	DirectX::SpriteFont* GetFont() const { return m_spFont.get(); }

private:
	std::unique_ptr<DirectX::SpriteFont> m_spFont = nullptr;
};