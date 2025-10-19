#include "../inc_KdCommon.hlsli"

//CPUから送られる情報
cbuffer cbWipe : register(b0)
{
	float g_progress;
	float3 _padding3;
}

//ノイズテクスチャ
Texture2D g_noiseTex : register(t0);
SamplerState g_sampler : register(s0);

float4 main(float4 screenPos : SV_Position) : SV_Target0
{
	//return float4(g_progress, 0, 0, 1);
	
	//ピクセルのスクリーン座標からUV座標計算
	float2 screenUV = screenPos.xy / g_screenSize.xy;

	//Y座標反転
	screenUV.y = 1.0 - screenUV.y;

	//ノイズテクスチャから値取得
	float noiseValue = g_noiseTex.Sample(g_sampler, screenUV).r;

	 //ノイズの値が進捗度より小さければこのピクセルを捨てる
	if (noiseValue > g_progress)
	{
		clip(-1);
	}

	//何も捨てなければ真っ黒を返す
	return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
