struct PS_INPUT
{
	float4 SvPos : SV_Position;
	float4 Color : COLOR;
};

float4 main(PS_INPUT In) : SV_TARGET0
{
	//頂点シェーダから受け取った色をそのまま出力させるだけ
	return In.Color;
}
