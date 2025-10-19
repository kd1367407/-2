#include "BlockObject.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/RenderComponent/RenderComponent.h"
#include"../../main.h"

void BlockObject::Init()
{
	m_renderComponent = m_owner->GetComponent<RenderComponent>();
}

void BlockObject::Update()
{
	//ブロック固有の更新処理（例：選択されたときのエフェクトなど）
}

void BlockObject::OnStageStateChanged()
{
	//将来的に、入れ替え時にパーティクルを出すなどの演出をここに追加できる。
	Application::Instance().AddLog("%s received astate change notification!",m_owner->GetName().c_str());
}

//void BlockObject::SetModelInfo(const std::shared_ptr<StageModel>& model, int gridX, int gridY, int gridZ)
//{
//	m_wpStageModel = model;
//	m_gridX = gridX;
//	m_gridY = gridY;
//	m_gridZ = gridZ;
//
//	//最初の見た目を設定
//	UpdateAppearance();
//}

//void BlockObject::UpdateAppearance()
//{
//	if (auto spModel = m_wpStageModel.lock())
//	{
//		if (m_renderComponent)
//		{
//			BlockType type = spModel->GetBlockTypeAt(m_gridX, m_gridY, m_gridZ);
//			std::string modelPath = "";
//			switch (type)
//			{
//			case BlockType::Wall:
//				modelPath = "Asset/Data/Block/Wall.gltf";
//				break;
//			case BlockType::Movable:
//				modelPath = "Asset/Data/Block/Movable.gltf";
//				break;
//			case BlockType::Goal:
//				modelPath = "Asset/Data/Block/Goal.gltf";
//				break;
//			}
//
//			if (!modelPath.empty())
//			{
//				m_renderComponent->SetModel(KdAssets::Instance().m_modeldatas.GetData(modelPath));
//			}
//			else
//			{
//				//Emptyの場合はモデルを非表示に
//				m_renderComponent->SetModel(nullptr); 
//			}
//		}
//	}
//}
