#include "EditableObject.h"

void EditableObject::Init()
{
	//モデル読み込み
	m_spModel = std::make_shared<KdModelData>();
	m_spModel->Load("Asset/Data/Block/Block.gltf");

	//描画タイプ設定
	m_drawType = eDrawTypeLit;

	//初期座標
	SetPos({ 0.0f, 0.0f, 0.0f });

	//初期拡縮率
	SetScale(1.0f);
}

void EditableObject::DrawLit()
{
	if (m_spModel)
	{
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_mWorld);
	}
}

void EditableObject::OnImGui()
{
	//オブジェクト名を表示
	ImGui::Text(U8("名前: %s"), m_name.c_str());
	ImGui::Text(U8("種類: EditableObject"));

	//基底クラスのOnImGuiを呼び出して共通UIを描画
	KdGameObject::OnImGui();

	//以下にEditableObject固有のUIを追加
}
