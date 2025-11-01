#include "RenderComponent.h"
#include"../TransformComponent/TransformComponent.h"
//#include"../IdComponent/IdComponent.h"
#include"../GameObject.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void RenderComponent::Awake()
{
}

void RenderComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();
}

void RenderComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("RenderComponent"))return;

	const auto& renderData = data.at("RenderComponent");

	if (renderData.contains("model"))
	{
		std::string modelPath = JsonHelper::GetString(renderData, "model");
		if (!modelPath.empty())
		{
			SetModel(modelPath);
		}
	}
}

nlohmann::json RenderComponent::ToJson() const
{
	nlohmann::json j;
	j["model"] = m_modelPath;

	return j;
}

void RenderComponent::DrawLit()
{
	if (!m_enable) return;
	if (m_spModel && m_transform)
	{
		if (m_owner && m_owner->GetName()._Starts_with("Particle_"))
		{
			return;
		}

		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_transform->GetMatrix());
	}
}

void RenderComponent::DrawBright()
{
	if (!m_enable) return;
	//何かしたのハイライトが設定されている場合のみ描画
	if (m_highlightState != HighlightState::None && m_spModel && m_transform)
	{
		auto& shader = KdShaderManager::Instance().m_StandardShader;

		//現在のシェーダーのマテリアル情報を取得して保存
		KdStandardShader::cbMaterial originalMaterial = shader.GetMaterialCB();
		KdStandardShader::cbMaterial customMaterial = originalMaterial;

		//状況に応じてカスタムマテリアルの発光色を上書き
		if (m_highlightState == HighlightState::Selected)
		{
			//強く光らせる
			customMaterial.Emissive = { 1.5f,1.0f,0.0f };
		}
		else//Hoverd
		{
			//弱く光らせる
			customMaterial.Emissive = { 0.5f,0.5f,0.5f };
		}

		//上書きしたカスタムマテリアルをシェーダーに設定
		shader.SetMaterialCB(customMaterial);

		//カスタムマテリアルが設定された状態でモデルを描画
		KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_transform->GetMatrix());

		// 処理が終わったら、他のオブジェクトに影響しないようにマテリアル情報を元に戻す
		shader.SetMaterialCB(originalMaterial);
	}
}

void RenderComponent::SetModel(const std::shared_ptr<KdModelData>& model)
{
	m_spModel = model;
	if (model)
	{
		m_modelPath = model->GetFilePath();
	}
	else
	{
		m_modelPath.clear();
	}
}

void RenderComponent::SetModel(const std::string& path)
{
	if (!path.empty())
	{
		m_spModel = KdAssets::Instance().m_modeldatas.GetData(path);
		m_modelPath = path;
	}
	else
	{
		m_spModel.reset();
		m_modelPath.clear();
	}
}

void RenderComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Render Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (m_modelPath.empty())
		{
			ImGui::Text("Model Path: N/A");
		}
		else
		{
			std::string_view path_view = m_modelPath;
			ImGui::InputText("Model Path", (char*)path_view.data(), path_view.size(), ImGuiInputTextFlags_ReadOnly);
			//ImGui::TextWrapped("Model Path", (char*)path_view.data(), path_view.size(), ImGuiInputTextFlags_ReadOnly);
		}
	}
}
