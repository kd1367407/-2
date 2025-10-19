#include "InputHandler.h"
#include"../InputReceiver/IInputReceiver.h"
#include"../Src/Application/Scene/SceneManager.h"//シーンオブジェクトにアクセスするため
#include"../Src/Application/Scene/BaseScene/BaseScene.h"
#include"../Src/Application/System/PhysicsSystem.h"
#include"../Src/Application/GameViewModel.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../../Component/CameraComponent/CameraComponent.h"
#include"../Src/Application/main.h"
#include"../Src/Framework/Component/RenderComponent/RenderComponent.h"
#include"../Src/Framework/Component/IdComponent/IdComponent.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"

void InputHandler::Update()
{
	//レイに必要なカメラ情報取得
	auto activeCameraComp = SceneManager::Instance().GetCurrentScene()->GetActiveCamera();
	if (!activeCameraComp)return;
	auto kdCamera = activeCameraComp->GetCamera();
	if (!kdCamera)return;

	//マウスのカーソルのスクリーン座標を取得
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(Application::Instance().GetWindowHandle(), &mousePos);

	//レイ生成
	RayInfo ray;
	kdCamera->GenerateRayInfoFromClientPos(mousePos, ray.m_start, ray.m_dir, ray.m_maxDistance);

	//--ホバー処理--
	std::shared_ptr<GameObject> currentHoveredObj = nullptr;
	// PhysicsSystemにレイキャストを依頼
		RayResult result;
	if (PhysicsSystem::Instance().Raycast(ray, result, CollisionLayer::LayerBlock))
	{
		//hitした
		if (auto hitObj = result.m_hitObject.lock())
		{
			//入れ替え可能オブジェクトか?
			if (auto viewModel = m_wpViewModel.lock())
			{
				if (auto idComp = hitObj->GetComponent<IdComponent>())
				{
					UINT id = idComp->GetId();
					BlockState state = viewModel->GetBlockState(id);

					if (state.isSwappable)
					{
						currentHoveredObj = hitObj;
					}
				}
			}
		}
	}
	auto preHoveredObj = m_wpHoverdObj.lock();

	//ホバー状態が変換した場合
	if (currentHoveredObj != preHoveredObj)
	{
		//前のオブジェクトのホバーを解除
		if (preHoveredObj)
		{
			if (auto renderComp = preHoveredObj->GetComponent<RenderComponent>())
			{
				//選択状態でない場合のみハイライトを解除
				if (renderComp->GetHighlightState() == RenderComponent::HighlightState::Hoverd)
				{
					renderComp->SetHighlightState(RenderComponent::HighlightState::None);
				}
			}
		}

		//新しいオブジェクトをホバー状態にする
		if (currentHoveredObj)
		{
			if (auto renderComp = currentHoveredObj->GetComponent<RenderComponent>())
			{
				//選択状態でない場合のみホバーにする
				if (renderComp->GetHighlightState() == RenderComponent::HighlightState::None)
				{
					renderComp->SetHighlightState(RenderComponent::HighlightState::Hoverd);
				}
			}
		}
	}
	m_wpHoverdObj = currentHoveredObj;


	//左クリックによる選択
	if (KdInputManager::Instance().IsPress("Select"))
	{
		if (auto viewModel = m_wpViewModel.lock())
		{
			if (currentHoveredObj)
			{
				viewModel->OnBlockSelected(currentHoveredObj);
			}
		}
	}
}
