//共通の定数バッファ
#include"../inc_KdCommon.hlsli"

//頂点シェーダへの入力
struct VS_INPUT
{
	//--スロット0:モデルの頂点データ--
	float3 ModelPos : POSITION;//マスター図形の頂点座標

	//--スロット1:インスタンスごとのデータ--
	row_major float4x4 InstanceWorld : INSTANCE_WORLD;//各パーティクルのワールド行列
	float4 InstanceColor : INSTANCE_COLOR;//各パーティクルの色
};

//ピクセルシェーダへの出力
struct VS_OUTPUT
{
	float4 SvPos : SV_Position;//最終的なスクリーン座標
	float4 Color : COLOR;//パーティクルの色
};

VS_OUTPUT main(VS_INPUT In)
{
	VS_OUTPUT Out = (VS_OUTPUT) 0;

	//マスターモデルの頂点位置をインスタンスごとのワールド行列で変換(パーティクルのようにすごい数の場合は行列計算もこっちでするべき)
	//これらがGPUインスタンシングと呼ばれる高速化技術
	float4 worldPos = mul(float4(In.ModelPos, 1.0f), In.InstanceWorld);

	//通常のビュー行列(カメラ基準の座標系)、射影行列(カメラ基準の座標を視界、つまり視錐台に収まる座標系)で変換
	Out.SvPos = mul(worldPos, g_mView);
	Out.SvPos = mul(Out.SvPos, g_mProj);

	//色をピクセルシェーダに渡す
	Out.Color = In.InstanceColor;

	return Out;
}
