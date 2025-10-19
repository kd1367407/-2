#include "MapBlock.h"

// 静的メンバ変数の実体を定義し、初期化
std::shared_ptr<KdModelData> MapBlock::s_spModel = nullptr;

void MapBlock::Init()
{
	if (s_spModel == nullptr)//モデルデータがロードされていなかった時
	{
		s_spModel = std::make_shared<KdModelData>();
		if (!s_spModel->Load("Asset/Data/Block/Block.gltf"))
		{
			s_spModel = nullptr;//失敗したらnullptrのまま
		}
	}

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("ExchangeCone", s_spModel, KdCollider::TypeGround);
}

void MapBlock::Update()
{
	Math::Matrix scaleMat;
	Math::Matrix rotMat;
	Math::Matrix transMat;
	
	transMat.CreateTranslation(m_pos);

	m_mWorld = transMat;
}

void MapBlock::DrawLit()
{
	if (s_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*s_spModel, GetMatrix());
	}
}

void MapBlock::SetParameter(const std::string& id, const Math::Vector3& pos)
{
	m_id = id;
	m_pos = pos;
}
