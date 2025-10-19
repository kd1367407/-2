#pragma once

//インスタンスデータ
struct ParticleInstance
{
	Math::Matrix world;
	Math::Color color;
};

class ParticleShader
{
public:
	bool Init();
	void Release();

	//インスタンス描画
	void Draw(const KdMesh* mesh, const KdBuffer& instanceBuffer, UINT instanceCount);

private:
	ID3D11VertexShader* m_vs = nullptr;
	ID3D11PixelShader* m_ps = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};