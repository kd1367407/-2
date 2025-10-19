#include "SkydomeComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/Scene/BaseScene/BaseScene.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../CameraComponent/ICameraComponent/ICameraComponent.h"

void SkydomeComponent::Awake()
{
	m_spModel = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Skydome/Skydome.gltf");
}

void SkydomeComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();
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

	//描画
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spModel, m_transform->GetMatrix());

	//元に戻す
	KdShaderManager::Instance().UndoRasterizerState();
	KdShaderManager::Instance().UndoDepthStencilState();
}
