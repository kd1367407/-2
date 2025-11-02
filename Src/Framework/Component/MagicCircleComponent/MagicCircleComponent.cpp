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
	m_localRot.y += m_rotationSpeedY * deltatime;
	m_localRot.y = fmod(m_localRot.y, 360.0f);
}

void MagicCircleComponent::DrawBright()
{
	if (!m_ownerTransform || !m_spModel)return;

	//行列計算
	Math::Matrix scaleMat = Math::Matrix::CreateScale(m_localScale);
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

	//KdShaderManager::Instance().m_StandardShader.SetEmissieEnable(true);

	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, finalMat);

	//KdShaderManager::Instance().m_StandardShader.SetEmissieEnable(false);
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
	JsonHelper::GetFloat(magicCircleData, "rotationSpeedY", m_rotationSpeedY);
}

nlohmann::json MagicCircleComponent::ToJson() const
{
	nlohmann::json j;
	j["model"] = m_modelPath;

	j["localPosition"] = { m_localPos.x, m_localPos.y, m_localPos.z };
	j["localRotation"] = { m_localRot.x, m_localRot.y, m_localRot.z };
	j["localScale"] = { m_localScale.x, m_localScale.y, m_localScale.z };
	j["rotationSpeedY"] = m_rotationSpeedY;

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

		//--Pos--
		valueChanged |= ImGui::DragFloat3("localPos", &m_localPos.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--Rot--
		valueChanged |= ImGui::DragFloat3("localRot", &m_localRot.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//--Scale--
		valueChanged |= ImGui::DragFloat3("localScale", &m_localScale.x, 0.1f);
		itemDeactivated |= ImGui::IsItemDeactivatedAfterEdit();

		//いずれかのウィジェットで値が変更されたとき
		if (valueChanged) { m_isDirty = true; }

		//いずれかのウィジェットウィジェットのドラッグが終了した瞬間
		if (itemDeactivated)
		{
			RequestTransformChangeCommand();
		}
	}
}
