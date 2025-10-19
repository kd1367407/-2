#include "inc_KdSpriteShader.hlsli"

// テクスチャ
Texture2D g_inputTex : register(t0);

Texture2D g_normalTex1 : register(t1);
Texture2D g_normalTex2 : register(t2);
// サンプラ
SamplerState g_ss : register(s0);


//============================================
// 2D描画 ピクセルシェーダ
//============================================
float4 main(VSOutput In) : SV_Target0
{
	if(g_gridEnable)
	{
		//1枚目のグリッドテクスチャをオフセット1を加えてサンプリング
		float4 color1 = g_normalTex1.Sample(g_ss, In.UV + g_UVOffset1);
		//2枚目も同様
		float4 color2 = g_normalTex2.Sample(g_ss, In.UV + g_UVOffset2);

		//color1 = color1 * 2.0 - 1.0;
		//color2 = color2 * 2.0 - 1.0;

		return (color1 + color2) * g_color;
	}
	else
	{
		// テクスチャ色取得
		float4 texCol = g_inputTex.Sample(g_ss, In.UV);
		//アルファテスト
		if (texCol.a < 0.1f)
			discard;
	
		// テクスチャ色 * 指定色
		return texCol * g_color;
	}
}
