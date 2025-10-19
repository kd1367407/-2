//#include "EditorInputHandler.h"
//#include"../Src/Application/Scene/GameScene/GameScene.h"
//#include"../../Editor/Editor.h"
//#include"../Src/Application/System/PhysicsSystem.h"
//#include"../Src/Application/main.h"
//#include"../../Component/CameraComponent/CameraComponent.h"
//
//void EditorInputHandler::Update(GameScene& scene, Editor& editor)
//{
//	// マウスがSceneViewウィンドウの上にあるか確認する必要があるため、
//   // Begin/Endでウィンドウのコンテキストを開始する
//	if (ImGui::Begin("Scene"))
//	{
//		//ウィンドウの上にマウスがあるかだけをチェック
//		if (ImGui::IsWindowHovered())
//		{
//			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
//			{
//				auto activeCamera = scene.GetActiveCamera();
//				if (activeCamera)
//				{
//					//SceneViewウィンドウの左上のスクリーン座標を取得
//					ImVec2 scenePos = ImGui::GetWindowPos();
//					//マウスのグローバルなスクリーン座標を取得
//					ImVec2 mousePos = ImGui::GetMousePos();
//
//					//SceneView内でのマウスのローカル座標を計算
//					POINT clientMousePos;
//					clientMousePos.x = static_cast<LONG>(mousePos.x - scenePos.x);
//					clientMousePos.y = static_cast<LONG>(mousePos.y - scenePos.y);
//
//					RayInfo ray;
//					Math::Vector3 rayPos, rayDir;
//					float rayRange;
//					activeCamera->GetCamera()->GenerateRayInfoFromClientPos(clientMousePos, rayPos, rayDir, rayRange);
//
//					ray.m_start = rayPos;
//					ray.m_dir = rayDir;
//					ray.m_maxDistance = rayRange;
//					RayResult result;
//
//					if (PhysicsSystem::Instance().Raycast(ray, result, LayerAll))
//					{
//						editor.SetSelectedObject(result.m_hitObject.lock());
//					}
//					else
//					{
//						editor.SetSelectedObject(nullptr);
//					}
//				}
//			}
//		}
//	}
//
//	ImGui::End();
//}
