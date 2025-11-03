#include "MagicCircleComponent.h"
#include"../TransformComponent/TransformComponent.h"
#include"../GameObject.h"
#include"../Src/Application/main.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include"../IdComponent/IdComponent.h"

void MagicCircleComponent::Awake()
{
}

void MagicCircleComponent::Start()
{
	m_ownerTransform = m_owner->GetComponent<TransformComponent>();
}

void MagicCircleComponent::Update()
{
	float deltatime = Application::Instance().GetDeltaTime();

	//公転
	m_orbitAngle += m_orbitSpeed * deltatime;
	m_orbitAngle = fmod(m_orbitAngle, 360.0f);

	float orbitRadians = DirectX::XMConvertToRadians(m_orbitAngle);

	m_localPos.x = m_orbitRadius * cos(orbitRadians) + m_orbitAxisOffset.x;
	m_localPos.z = m_orbitRadius * sin(orbitRadians) + m_orbitAxisOffset.z;
	m_localPos.y = m_orbitAxisOffset.y;

	//m_localRot.y = m_orbitAngle;

	float currentSpeed = m_isSelected ? m_selectedSpeed : m_normalSpeed;

	m_localRot.y += currentSpeed * deltatime;
	m_localRot.y = fmod(m_localRot.y, 360.0f);

	Math::Vector3 targetScale = m_localScale;
	if (m_isSelected)
	{
		targetScale *= m_selectionScaleMultiplier;
		targetScale *= m_selectionScaleMultiplier;
	}

	m_currentScale = Math::Vector3::Lerp(m_currentScale, targetScale, deltatime * m_scaleLerpSpeed);
}

void MagicCircleComponent::DrawLit()
{
	if (!m_ownerTransform || !m_spModel)return;

	//行列計算
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_currentScale);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_localRot.y),
		DirectX::XMConvertToRadians(m_localRot.x),
		DirectX::XMConvertToRadians(m_localRot.z)
	);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_localPos);

	Math::Matrix localMat = scaleMat * rotMat * transMat;
	const Math::Matrix& ownerWorldMat = m_ownerTransform->GetMatrix();

	//最終的な行列
	Math::Matrix finalMat = localMat * ownerWorldMat;

	//元のマテリアルをバックアップ
	KdStandardShader::cbMaterial originalMaterial = KdShaderManager::Instance().m_StandardShader.GetMaterialCB();

	//発光を強くする
	KdStandardShader::cbMaterial customMaterial = originalMaterial;
	customMaterial.Emissive = { 100.0f,100.0f,100.0f };

	//カスタムマテリアルをセット
	KdShaderManager::Instance().m_StandardShader.SetMaterialCB(customMaterial);
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, finalMat);

	KdShaderManager::Instance().m_StandardShader.SetMaterialCB(originalMaterial);
	KdShaderManager::Instance().UndoRasterizerState();
}

void MagicCircleComponent::DrawBright()
{
	if (!m_ownerTransform || !m_spModel)return;

	//行列計算
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_currentScale);
	Math::Matrix rotMat = Math::Matrix::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_localRot.y),
		DirectX::XMConvertToRadians(m_localRot.x),
		DirectX::XMConvertToRadians(m_localRot.z)
	);
	Math::Matrix transMat = Math::Matrix::CreateTranslation(m_localPos);

	Math::Matrix localMat = scaleMat * rotMat * transMat;
	const Math::Matrix& ownerWorldMat = m_ownerTransform->GetMatrix();

	//最終的な行列
	Math::Matrix finalMat = localMat * ownerWorldMat;

	//元のマテリアルをバックアップ
	KdStandardShader::cbMaterial originalMaterial = KdShaderManager::Instance().m_StandardShader.GetMaterialCB();

	//発光を強くする
	KdStandardShader::cbMaterial customMaterial = originalMaterial;
	//customMaterial.Emissive = { 5.0f,5.0f,5.0f };
	customMaterial.Emissive = { 100.0f,100.0f,100.0f };

	//カスタムマテリアルをセット
	KdShaderManager::Instance().m_StandardShader.SetMaterialCB(customMaterial);
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, finalMat);

	KdShaderManager::Instance().m_StandardShader.SetMaterialCB(originalMaterial);
	KdShaderManager::Instance().UndoRasterizerState();
}

void MagicCircleComponent::OnSelect(bool isSelected)
{
	m_isSelected = isSelected;
}

void MagicCircleComponent::SetModel(const std::string& path)
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

void MagicCircleComponent::RequestTransformChangeCommand()
{

	if (auto viewModel = m_wpViewModel.lock())
	{
		if (auto idComp = m_owner->GetComponent<IdComponent>())
		{
			viewModel->UpdateStateFromGameObject(m_owner->shared_from_this());
		}
	}
}

void MagicCircleComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("MagicCircleComponent"))return;

	const auto& magicCircleData = data.at("MagicCircleComponent");

	if (magicCircleData.contains("model"))
	{
		std::string modelPath = JsonHelper::GetString(magicCircleData, "model");
		if (!modelPath.empty())
		{
			SetModel(modelPath);
		}
	}

	JsonHelper::GetVector3(magicCircleData, "localPos", m_localPos, m_localPos);
	JsonHelper::GetVector3(magicCircleData, "localRot", m_localRot, m_localRot);
	JsonHelper::GetVector3(magicCircleData, "localScale", m_localScale, m_localScale);
	
	m_orbitRadius = JsonHelper::GetFloat(magicCircleData, "orbitRadius", m_orbitRadius);
	m_orbitSpeed = JsonHelper::GetFloat(magicCircleData, "orbitSpeed", m_orbitSpeed);
	JsonHelper::GetVector3(magicCircleData, "orbitAxisOffset", m_orbitAxisOffset, m_orbitAxisOffset);

	m_normalSpeed = JsonHelper::GetFloat(magicCircleData, "normalSpeed", m_normalSpeed);
	m_selectedSpeed = JsonHelper::GetFloat(magicCircleData, "selectedSpeed", m_selectedSpeed);
	m_selectionScaleMultiplier = JsonHelper::GetFloat(magicCircleData, "selectedScaleMultiplier", m_selectionScaleMultiplier);
	m_scaleLerpSpeed = JsonHelper::GetFloat(magicCircleData, "scaleLerpSpeed", m_scaleLerpSpeed);
}

nlohmann::json MagicCircleComponent::ToJson() const
{
	nlohmann::json j;
	j["model"] = m_modelPath;

	j["localPos"] = { m_localPos.x, m_localPos.y, m_localPos.z };
	j["localRot"] = { m_localRot.x, m_localRot.y, m_localRot.z };
	j["localScale"] = { m_localScale.x, m_localScale.y, m_localScale.z };

	j["orbitRadius"] = m_orbitRadius;
	j["orbitSpeed"] = m_orbitSpeed;
	j["orbitAxisOffset"] = { m_orbitAxisOffset.x, m_orbitAxisOffset.y, m_orbitAxisOffset.z };

	j["normalSpeed"] = m_normalSpeed;
	j["selectedSpeed"] = m_selectedSpeed;
	j["selectedScaleMultiplier"] = m_selectionScaleMultiplier;
	j["scaleLerpSpeed"] = m_scaleLerpSpeed;

	return j;
}

void MagicCircleComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("MagicCircle Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (m_modelPath.empty())
		{
			ImGui::Text("Model Path: N/A");
		}
		else
		{
			std::string_view path_view = m_modelPath;
			ImGui::InputText("Model Path", (char*)path_view.data(), path_view.size(), ImGuiInputTextFlags_ReadOnly);
		}

		bool valueChanged = false;
		bool itemDeactivated = false;

		//--相対Transform--
		//--Pos--
		valueChanged |= ImGui::DragFloat3("localPos", &m_localPos.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--Rot--
		valueChanged |= ImGui::DragFloat3("localRot", &m_localRot.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--Scale--
		valueChanged |= ImGui::DragFloat3("localScale", &m_localScale.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		ImGui::Separator();

		//--公転パラメータ--
		ImGui::Text("Orbit Parameters");
		valueChanged |= ImGui::DragFloat("orbitRadius", &m_orbitRadius, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		valueChanged |= ImGui::DragFloat("orbitSpeed", &m_orbitSpeed, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		valueChanged |= ImGui::DragFloat3("orbitAxisOffset", &m_orbitAxisOffset.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		ImGui::Separator();

		//--アニメーション関係--
		ImGui::Text("Animation Parameters");
		valueChanged |= ImGui::DragFloat("normalSpeed", &m_normalSpeed, 1.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		valueChanged |= ImGui::DragFloat("selectedSpeed", &m_selectedSpeed, 1.0f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		valueChanged |= ImGui::DragFloat("selectedScaleMultiplier", &m_selectionScaleMultiplier, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		valueChanged |= ImGui::DragFloat("scaleLerpSpeed", &m_scaleLerpSpeed, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//いずれかのウィジェットウィジェットのドラッグが終了した瞬間
		if (itemDeactivated)
		{
			RequestTransformChangeCommand();
		}
	}
}
