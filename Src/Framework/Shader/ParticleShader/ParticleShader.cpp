#include "ParticleShader.h"

bool ParticleShader::Init()
{
	//シェーダをコンパイルして読み込み
	{
#include"../Src/Framework/Shader/ParticleShader/Particle_VS.shaderInc"
		if (FAILED(KdDirect3D::Instance().WorkDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_vs)))
		{
			assert(0 && "パーティクルシェーダー(VS)の作成に失敗");
			Release();
			return false;
		}
	}

	{
#include"../Src/Framework/Shader/ParticleShader/Particle_PS.shaderInc"
		if (FAILED(KdDirect3D::Instance().WorkDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_ps)))
		{
			assert(0 && "パーティクルシェーダー(PS)の作成に失敗");
			Release();
			return false;
		}
	}

	{
#include"../Src/Framework/Shader/ParticleShader/Particle_VS.shaderInc"
		//2種類の頂点データを定義する入力レイアウト
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
		{
			//--スロット0:モデルの頂点データ--
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},

			//--スロット1:インスタンスごとのデータ--
			//D3D11_INPUT_PER_INSTANCE_DATAが頂点ごとではなく、インスタンス(コピー)ごとにデータを読み込み位置を1つ進めて下さいとGPUに教える
			{ "INSTANCE_WORLD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE_WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE_WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE_WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
			{ "INSTANCE_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		};

		if (FAILED(KdDirect3D::Instance().WorkDev()->CreateInputLayout(&layout[0], layout.size(), compiledBuffer, sizeof(compiledBuffer), &m_inputLayout))) {
			Release();
			return false;
		}
	}

	return true;
}

void ParticleShader::Release()
{
	KdSafeRelease(m_vs);
	KdSafeRelease(m_ps);
	KdSafeRelease(m_inputLayout);
}

void ParticleShader::Draw(const KdMesh* mesh, const KdBuffer& instanceBuffer, UINT instanceCount)
{
	if (!mesh || instanceCount == 0)return;

	//描画パイプラインを設定
	KdShaderManager::Instance().SetVertexShader(m_vs);
	KdShaderManager::Instance().SetPixelShader(m_ps);
	KdShaderManager::Instance().SetInputLayout(m_inputLayout);

	//頂点バッファセット
	mesh->SetToDevice_Instancing(instanceBuffer.GetBuffer(), sizeof(ParticleInstance));

	//インスタンス描画
	KdDirect3D::Instance().WorkDevContext()->DrawIndexedInstanced(
		mesh->GetIndexCount(),
		instanceCount,
		0, 0, 0
	);

	auto& shaderManager = KdShaderManager::Instance();
	shaderManager.m_StandardShader.BeginLit();
}
