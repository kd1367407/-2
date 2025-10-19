#include "BaseScene.h"
#include"../Src/Framework/Component/CameraComponent/CameraComponent.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/TransformComponent/TransformComponent.h"
#include"../Src/Framework/Component/RenderComponent/RenderComponent.h"
#include"../../main.h"
#include"../../System/PhysicsSystem.h"

void BaseScene::PreUpdate()
{
	//寿命切れのオブジェクトをリストから除外
	for (auto it = m_objList.begin(); it != m_objList.end();)
	{
		if ((*it)->IsExpired())
		{
			it = m_objList.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (const auto& obj : m_objList)
	{
		obj->PreUpdate();
	}
}

void BaseScene::Update()
{
	//シーン固有の更新処理
	SceneUpdate();

	for (const auto& obj : m_objList)
	{
		obj->Update();
	}
}

void BaseScene::PostUpdate()
{
	float deltaTime = Application::Instance().GetDeltaTime();
	if (deltaTime > 0.0f)
	{
		PhysicsSystem::Instance().Update(Application::Instance().GetDeltaTime());
	}

	for (const auto& obj : m_objList)
	{
		obj->PostUpdate();
	}
}

void BaseScene::PreDraw()
{
	if (auto spActiveCamera = m_wpActiveCamera.lock())
	{
		if (const auto& kdCam = spActiveCamera->GetCamera())
		{
			kdCam->SetToShader();
		}
	}
}

void BaseScene::Draw()
{
	// 3Dオブジェクトを描画する直前に、光と霧の情報をシェーダーに送る
	KdShaderManager::Instance().WorkAmbientController().Draw();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光を遮るオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginGenerateDepthMapFromLight();
	{
		for (auto& obj : m_objList)
		{
			obj->GenerateDepthMapFromLight();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndGenerateDepthMapFromLight();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のないオブジェクトはBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginUnLit();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawUnLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndUnLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 陰影のあるオブジェクト(不透明な物体や2Dキャラ)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_StandardShader.BeginLit();
	{

		for (auto& obj : m_objList)
		{
			obj->DrawLit();
		}
	}
	KdShaderManager::Instance().m_StandardShader.EndLit();

	// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== =====
	// 光源オブジェクト(自ら光るオブジェクトやエフェクト)はBeginとEndの間にまとめてDrawする
	KdShaderManager::Instance().m_postProcessShader.BeginBright();
	{
		for (auto& obj : m_objList)
		{
			obj->DrawBright();
		}
	}
	KdShaderManager::Instance().m_postProcessShader.EndBright();
}

void BaseScene::PostDraw()
{
	for (auto& obj : m_objList)
	{
		obj->PostDraw();
	}
}

void BaseScene::DrawSprite()
{
	KdShaderManager::Instance().m_spriteShader.Begin();
	for (auto& obj : m_objList)
	{
		obj->DrawSprite();
	}
	KdShaderManager::Instance().m_spriteShader.End();
}

void BaseScene::AddObject(const std::shared_ptr<GameObject>& obj)
{
	if (obj)
	{
		m_objList.push_back(obj);
	}
}

void BaseScene::SetActiveCamera(const std::shared_ptr<ICameraComponent>& camera)
{
	if (camera)
	{
		m_wpActiveCamera = camera;
	}
}

std::shared_ptr<ICameraComponent> BaseScene::GetActiveCamera()
{
	return m_wpActiveCamera.lock();
}

std::shared_ptr<GameObject> BaseScene::FindObject(std::string objName)const
{
	auto it = std::find_if(m_objList.begin(), m_objList.end(), [&](const std::shared_ptr<GameObject>& obj)
		{
			return obj && obj->GetName() == objName;
		}
	);

	if (it != m_objList.end())
	{
		return *it;
	}

	return nullptr;
}
