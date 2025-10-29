#include "MyFont.h"

static std::wstring ConvertToWString(std::string_view str)//std::string (UTF-8) を std::wstring (UTF-16) に変換する
{
	if (str.empty()) return std::wstring();

	//必要なバッファサイズ取得
	int charsNeeded = MultiByteToWideChar(
		CP_UTF8,//ソースはUTF-8
		0,//フラグ
		str.data(),//ソース文字列
		(int)str.size(),//ソース文字列サイズ
		NULL,//NULL
		0//0 = サイズだけ計算
	);

	if (charsNeeded == 0)return std::wstring();

	//変換実行
	std::wstring wstr(charsNeeded, 0);
	MultiByteToWideChar(
		CP_UTF8,
		0,
		str.data(),
		(int)str.size(),
		&wstr[0],//変換先バッファ
		charsNeeded//バッファサイズ
	);

	return wstr;
}

bool MyFont::Load(std::string_view fileName)
{
	//デバイス取得
	ID3D11Device* device = KdDirect3D::Instance().WorkDev();
	if (!device)return false;

	std::wstring wFileName = ConvertToWString(fileName);
	if (wFileName.empty())return false;

	try
	{
		m_spFont = std::make_unique<DirectX::SpriteFont>(device, wFileName.c_str());
	}
	catch (...)
	{
		//読み込み失敗
		assert(0 && "SpriteFont の読み込みに失敗しました。ファイルパス、または .spritefont ファイルが正しいか確認してください。");
		return false;
	}

	return true;
}

DirectX::XMVECTOR MyFont::MeasureString(const char* text) const
{
	if (m_spFont)
	{
		return m_spFont->MeasureString(text);
	}

	return DirectX::XMVectorZero();
}
