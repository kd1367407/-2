#include "SkydomeComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/Scene/BaseScene/BaseScene.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../CameraComponent/ICameraComponent/ICameraComponent.h"

void SkydomeComponent::Awake()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Skydome/Skydome1.gltf");
	m_gridTex1 = KdAssets::Instance().m_textures.GetData("Asset/Textures/grid3.png");
	m_gridTex2 = KdAssets::Instance().m_textures.GetData("Asset/Textures/grid3.png");
}

void SkydomeComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();
}

void SkydomeComponent::Update()
{
	float deltaTime = Application::Instance().GetDeltaTime();

	m_uvOffset1.y += 0.02f * deltaTime;
	//m_uvOffset2.x += 0.02f * deltaTime;

	if (m_uvOffset1.y > 1.0f) m_uvOffset1.y -= 1.0f;
	if (m_uvOffset2.y > 1.0f) m_uvOffset2.y -= 1.0f;
}

void SkydomeComponent::PostUpdate()
{
	if (auto camera = SceneManager::Instance().GetCurrentScene()->GetActiveCamera())
	{
		Math::Vector3 camPos = camera->GetCamera()->GetCameraMatrix().Translation();

		if (m_transform)
		{
			camPos.y -= 20.0f;
			m_transform->SetPos(camPos);
		}
	}
}

void SkydomeComponent::DrawUnLit()
{
	if (!m_spModel || !m_transform) return;

	//--スカイドーム用の描画設定--
	//zバッファへの書き込みOFF
	KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZWriteDisable);
	//裏面カリンングをOFF
	KdShaderManager::Instance().ChangeRasterizerState(KdRasterizerState::CullNone);

	KdShaderManager::Instance().m_StandardShader.SetUVTiling({ 5.0f, 5.0f });

	KdShaderManager::Instance().m_StandardShader.SetGridUVOffset(m_uvOffset1, m_uvOffset1);

	KdShaderManager::Instance().m_StandardShader.SetGridTexture(*m_gridTex1, *m_gridTex2);

	KdShaderManager::Instance().m_StandardShader.SetGridEnable(true);

	//描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_transform->GetMatrix());
	//KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_transform->GetMatrix(), kWhiteColor, Math::Vector3::One);

	KdShaderManager::Instance().m_StandardShader.SetGridEnable(false);

	//元に戻す
	KdShaderManager::Instance().UndoRasterizerState();
	KdShaderManager::Instance().UndoDepthStencilState();
}
