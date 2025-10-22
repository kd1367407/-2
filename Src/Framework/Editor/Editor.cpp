#include "Editor.h"
#include"../Component/GameObject.h"
#include"../Src/Application/Scene/GameScene/GameScene.h"
#include"../Component/ImGuiComponent/ImGuiComponent.h"
#include"../GameObject/ArchetypeManager.h"
#include"../Component/IdComponent/IdComponent.h"
#include"../Src/Application/System/PhysicsSystem.h"
#include"../Component/CameraComponent/EditorCameraComponent/EditorCameraComponent.h"
#include"../Src/Application/main.h"
#include"../Component/TransformComponent/TransformComponent.h"
#include"../Component/JumpBlockComponent/JumpBlockComponent.h"
#include"../Component/MovingBlockComponent/MovingBlockComponent.h"
#include"../Component/BlockDataComponent/BlockDataComponent.h"
#include"../Component/SinkingBlockComponent/SinkingBlockComponent.h"
#include"../Component/ScalingBlockComponent/ScalingBlockComponent.h"
#include"../Component/RotatingBlockComponent/RotatingBlockComponent.h"
#include"../Component/SolutionVisualizerComponent/SolutionVisualizerComponent.h"
#include"../Component/CameraComponent/ICameraComponent/ICameraComponent.h"
#include"../Component/SlipperyComponent/SlipperyComponent.h"
#include"../Component/JumpBlockComponent/JumpBlockComponent.h"
#include"../Component/TagComponent/TagComponent.h"
#include"EditorGizmoContext.h"
#include"../Src/Application/Scene/GameScene/GameManager/GameManager.h"

void Editor::Init()
{
	//画面と同じサイズのレンダーターゲットを作成
	m_sceneRT.CreateRenderTarget(KdDirect3D::Instance().GetBackBuffer()->GetWidth(), KdDirect3D::Instance().GetBackBuffer()->GetHeight(), true);
}

void Editor::Draw(GameScene& scene)
{
	//Gizmoのフレーム開始を伝える
	ImGuizmo::BeginFrame();

	//全てのウィンドウ描画を呼び出す
	DrawHierarchyWindow(scene);
	DrawInspectorWindow(scene);
	DrawSceneViewWindow(scene);
	DrawSwapToolWindow(scene);

	if (auto visualizer = scene.FindObject("SolutionVisualizer"))
	{
		if (auto comp = visualizer->GetComponent<SolutionVisualizerComponent>())
		{
			comp->SetShouldDraw(m_bShowSolutionPath);
			comp->SetViewMode(m_solutionViewMode);
		}
	}
}

void Editor::DrawHierarchyWindow(GameScene& scene)
{
	ImGui::Begin("Hierarchy");

	//ステージ情報
	ImGui::SeparatorText("Stage Info");

	auto& gm = GameManager::Instance();
	static char stageNameBuffer[256];
	strncpy_s(stageNameBuffer, sizeof(stageNameBuffer), gm.GetCurrentStageLabel().c_str(), _TRUNCATE);
	if (ImGui::InputText("Name", stageNameBuffer, sizeof(stageNameBuffer)))
	{
		gm.SetCurrentStageLabel(stageNameBuffer);
	}

	//保存
	if (ImGui::Button("Save Stage"))
	{
		//保存関数呼び出し
		scene.RequestSaveStage();

		//アンドゥ履歴をリセット
		scene.UndoClearFrimEditor();
	}
	ImGui::Separator();

	//タイトルへ
	if (ImGui::Button("Return Title"))
	{
		//保存関数呼び出し
		scene.RequestSaveStage();

		//アンドゥ履歴をリセット
		scene.UndoClearFrimEditor();

		//タイトルへ
		SceneManager::Instance().ChangeScene(SceneManager::SceneType::Title);
	}
	ImGui::Separator();

	//解法ビジュアライザー
	ImGui::SeparatorText("Solution Path");

	if (auto viewModel = scene.GetViewModel())
	{
		if (!viewModel->IsSolutionRecording())
		{
			if (ImGui::Button("Start Recording"))
			{
				viewModel->StartSolutionRecording();
			}
		}
		//レコーディング中ならstopボタン表示
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));//赤色
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));//上より彩度が高い赤色
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));//上より更に彩度が高い赤色

			if (ImGui::Button("Stop Recording"))
			{
				viewModel->StopSolutionRecording();
			}

			ImGui::PopStyleColor(3);
		}

		//パスクリア
		if (ImGui::Button("Clear Path"))
		{
			viewModel->ClearSolutionPath();
		}

		//パスの表示切り変え
		ImGui::Checkbox("Show Path", &m_bShowSolutionPath);

		if (m_bShowSolutionPath)
		{
			if (ImGui::RadioButton("Static View", m_solutionViewMode == SolutionViewMode::Static))
			{
				m_solutionViewMode = SolutionViewMode::Static;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Animate View", m_solutionViewMode == SolutionViewMode::Animate))
			{
				m_solutionViewMode = SolutionViewMode::Animate;
			}

			//再生コントロールUI
			ImGui::Separator();
			if (ImGui::Button("Play"))
			{
				if (auto visualizer = scene.FindObject("SolutionVisualizer"))
				{
					if (auto comp = visualizer->GetComponent<SolutionVisualizerComponent>())
					{
						comp->Play();
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Pause"))
			{
				if (auto visualizer = scene.FindObject("SolutionVisualizer"))
				{
					if (auto comp = visualizer->GetComponent<SolutionVisualizerComponent>())
					{
						comp->Pause();
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Reset"))
			{
				if (auto visualizer = scene.FindObject("SolutionVisualizer"))
				{
					if (auto comp = visualizer->GetComponent<SolutionVisualizerComponent>())
					{
						comp->Reset();
					}
				}
			}
		}
	}

	//--オブジェクトを生成--
	ImGui::SeparatorText("Create Object");

	//ArchetypeManagerから作成可能なオブジェクト名の一覧を取得
	const auto archetypeNames = ArchetypeManager::Instance().GetAllArchetypeNames();

	//string -> char*へ変換(ImGuiがchar*を扱うから)
	std::vector<const char*> namesC_str;
	namesC_str.reserve(archetypeNames.size());
	for (const std::string& name : archetypeNames)
	{
		namesC_str.push_back(name.c_str());
	}

	//現在選択されている項目
	static int currentItem = 0;
	ImGui::Combo("Archetype", &currentItem, namesC_str.data(), static_cast<int>(namesC_str.size()));

	if (ImGui::Button("Create"))
	{
		if (currentItem < archetypeNames.size())
		{
			auto createdObjects = scene.GetViewModel()->CreateObjectForEditor(archetypeNames[currentItem]);

			if (!createdObjects.empty())
			{
				m_selectedObjects.clear();
				m_selectedObjects.push_back(createdObjects.front());
			}
		}
	}

	ImGui::Spacing();
	ImGui::SeparatorText("Create Group");

	if (ImGui::Button("Create Group from Selection"))
	{
		if (!m_selectedObjects.empty())
		{
			ImGui::OpenPopup("Create New Group");
		}
	}

	//グループ作成用のモーダルポップアップ
	if (ImGui::BeginPopupModal("Create New Group", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char groupNameBuffer[256] = "New Group";
		ImGui::InputText("Group Name", groupNameBuffer, sizeof(groupNameBuffer));

		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			ObjectGroup newGroup;
			newGroup.name = groupNameBuffer;
			for (const auto& weak_obj : m_selectedObjects)
			{
				if (auto obj = weak_obj.lock())
				{
					if (auto idComp = obj->GetComponent<IdComponent>())
					{
						newGroup.memberObjectIDs.push_back(idComp->GetId());
					}
				}
			}
			m_groups.push_back(newGroup);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	//作成されたグループの一覧
	if (!m_groups.empty())
	{
		ImGui::SeparatorText("Created Group");

		for (int i = 0; i < m_groups.size(); ++i)
		{
			if (ImGui::Selectable(m_groups[i].name.c_str(), i == m_selectedGroupIndex))
			{
				//クリックされたグループのインデックス(vectorの何番目か)を記録
				m_selectedGroupIndex = i;

				//グループ名がクリックされたらメンバーを選択状態に
				m_selectedObjects.clear();
				const auto& allObjects = scene.GetObjList();

				for (UINT targetID : m_groups[i].memberObjectIDs)
				{
					for (const auto& obj : allObjects)
					{
						if (auto idComp = obj->GetComponent<IdComponent>())
						{
							if (idComp->GetId() == targetID)
							{
								m_selectedObjects.push_back(obj);
								break;
							}
						}
					}
				}
			}
		}
	}

	ImGui::SeparatorText("Scene Objects");

	bool isCtrlDown = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

	//シーンのオブジェクト一覧を取得、表示
	for (const auto& obj : scene.GetObjList())
	{
		if (!obj)return;

		if (obj->GetComponent<TagComponent>())
		{
			continue;
		}

		//このオブジェクトが現在選択されているかチェック
		bool isSelected = false;
		for (const auto& weak_selected : m_selectedObjects)
		{
			if (auto selected = weak_selected.lock())
			{
				if (selected == obj)
				{
					isSelected = true;
					break;
				}
			}
		}
		
		//オブジェクト名を表示
		if (ImGui::Selectable(obj->GetName().c_str(), isSelected))
		{
			//Ctrlが押されていない場合
			if (!isCtrlDown)
			{
				m_selectedObjects.clear();
				m_selectedObjects.push_back(obj);

				auto cameraComp = obj->GetComponent<ICameraComponent>();
				if (cameraComp)
				{
					scene.SetActiveCamera(cameraComp);
				}
			}
			//Ctrlが押されている場合
			else
			{
				//すでに選択されている場合
				if (isSelected)
				{
					std::erase_if(m_selectedObjects, [&](const auto& weak_selected)
						{
							return weak_selected.lock() == obj;
						}
					);
				}
				//選択されていない場合
				else
				{
					m_selectedObjects.push_back(obj);
				}
			}
		}
	}

	ImGui::End();
}

void Editor::DrawInspectorWindow(GameScene& scene)
{
	ImGui::Begin("Inspector");

	//無効なポインタを削除
	std::erase_if(m_selectedObjects, [](const auto& wp) {
		return wp.expired();
		}
	);

	//--コピペ--
	bool isCtrlDown = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

	//コピー(Ctrl+C)
	if (isCtrlDown && ImGui::IsKeyPressed(ImGuiKey_C))
	{
		if (!m_selectedObjects.empty())
		{
			m_clipboard.clear();

			for (const auto& weak_obj : m_selectedObjects)
			{
				if (auto obj = weak_obj.lock())
				{
					m_clipboard.push_back(obj->CreateState());
				}
			}

			Application::Instance().AddLog("%zu object(s) copied.", m_clipboard.size());
		}
	}

	//ペースト(Ctrl+V)
	if (isCtrlDown && ImGui::IsKeyPressed(ImGuiKey_V))
	{
		if (!m_clipboard.empty())
		{
			if (auto viewModel = scene.GetViewModel())
			{
				auto pastedObjects = viewModel->CreateObjectsFromClipboard(m_clipboard);

				m_selectedObjects.clear();
				for (const auto& obj : pastedObjects)
				{
					m_selectedObjects.push_back(obj);
				}
				Application::Instance().AddLog("%zu object(s) pasted.", pastedObjects.size());
			}
		}
	}

	if (m_selectedObjects.empty())
	{
		ImGui::Text("No Object Selected");
	}
	//単一選択の場合
	else if (m_selectedObjects.size() == 1)
	{
		if (auto selected = m_selectedObjects[0].lock())
		{
			//ギズモの操作モードを切り替える
			if (ImGui::RadioButton("Translate", m_currentGizmoOperation == ImGuizmo::TRANSLATE))
			{
				m_currentGizmoOperation = ImGuizmo::TRANSLATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Rotate", m_currentGizmoOperation == ImGuizmo::ROTATE))
			{
				m_currentGizmoOperation = ImGuizmo::ROTATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Scale", m_currentGizmoOperation == ImGuizmo::SCALE))
			{
				m_currentGizmoOperation = ImGuizmo::SCALE;
			}

			//--削除--
			if (ImGui::Button("Delete Object"))
			{
				scene.GetViewModel()->RequestDeleteObject(selected);
				m_selectedObjects.clear();
			}

			if (ImGui::Button("Undo"))
			{
				//アンドゥ実行
				scene.Undo();

				Application::Instance().AddLog("Undo");
			}

			ImGui::Separator();

			if (auto imGuiComp = selected->GetComponent<ImGuiComponent>())
			{
				//ImguiCompoentに描画丸投げ
				imGuiComp->DrawImGui();
			}
		}
	}
	//複数選択の場合
	else
	{
		ImGui::SeparatorText("Gizmo Operation");

		//ギズモの操作モードを切り替える
		if (ImGui::RadioButton("Translate", m_currentGizmoOperation == ImGuizmo::TRANSLATE))
		{
			m_currentGizmoOperation = ImGuizmo::TRANSLATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", m_currentGizmoOperation == ImGuizmo::ROTATE))
		{
			m_currentGizmoOperation = ImGuizmo::ROTATE;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", m_currentGizmoOperation == ImGuizmo::SCALE))
		{
			m_currentGizmoOperation = ImGuizmo::SCALE;
		}

		ImGui::Text("%zu Objexts selected", m_selectedObjects.size());
		ImGui::Separator();

		//Inspectorの表示モードを切り替え
		ImGui::SeparatorText("Inspector Mode");
		if (ImGui::RadioButton("Gizmo Transform", m_multiEditMode == MultiEditMode::Gizmo))
		{
			m_multiEditMode = MultiEditMode::Gizmo;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Batch Edit Components", m_multiEditMode == MultiEditMode::Batch))
		{
			m_multiEditMode = MultiEditMode::Batch;
		}
		ImGui::Separator();

		if (m_multiEditMode == MultiEditMode::Gizmo)
		{
			DrawMultiGizmoTransform();
		}
		else
		{
			//強い参照リストを一度だけ生成
			std::vector<std::shared_ptr<GameObject>> strongSelectedObjs;
			for (auto& weak_obj : m_selectedObjects)
			{
				if (auto obj = weak_obj.lock())
				{
					strongSelectedObjs.push_back(obj);
				}
			}

			if (strongSelectedObjs.empty())
			{
				ImGui::End();
				return;
			}

			//--特有パラメータ--
			bool hasDrawSpecialUI = false;

			//Moving
			bool allHaveMovingBlock = true;
			for (const auto& obj : strongSelectedObjs)
			{
				if (!obj->GetComponent<MovingBlockComponent>())
				{
					allHaveMovingBlock = false;
					break;
				}
			}

			if (allHaveMovingBlock)
			{
				DrawMultiEditMovingBlock(scene, m_selectedObjects);
				hasDrawSpecialUI = true;
			}

			//Sinking
			bool allHaveSinkingBlock = true;
			for (const auto& obj : strongSelectedObjs)
			{
				if (!obj->GetComponent<SinkingBlockComponent>())
				{
					allHaveSinkingBlock = false;
					break;
				}
			}

			if (allHaveSinkingBlock)
			{
				DrawMultiEditSinkingBlock(scene, m_selectedObjects);
				hasDrawSpecialUI = true;
			}

			//Scaling
			bool allHavescalingBlock = true;
			for (const auto& obj : strongSelectedObjs)
			{
				if (!obj->GetComponent<ScalingBlockComponent>())
				{
					allHavescalingBlock = false;
					break;
				}
			}

			if (allHavescalingBlock)
			{
				DrawMultiEditScalingBlock(scene, m_selectedObjects);
				hasDrawSpecialUI = true;
			}

			//Rotating
			bool allHaveRotatingBlock = true;
			for (const auto& obj : strongSelectedObjs)
			{
				if (!obj->GetComponent<RotatingBlockComponent>())
				{
					allHaveRotatingBlock = false;
					break;
				}
			}

			if (allHaveRotatingBlock)
			{
				DrawMultiEditRotatingBlock(scene, m_selectedObjects);
				hasDrawSpecialUI = true;
			}

			//他の特殊パラメータコンポーネントも

			//共通パラメータ
			if (!hasDrawSpecialUI)
			{
				DrawMultiEditTransform(scene, m_selectedObjects);
			}

			//Slippery
			bool allHaveSlippery = true;
			for (const auto& obj : strongSelectedObjs)
			{
				if (!obj->GetComponent<SlipperyComponent>())
				{
					allHaveSlippery = false;
					break;
				}
			}
			if (allHaveSlippery)
			{
				DrawMultiEditSlipperyBlock(scene, m_selectedObjects);
			}

			//Jump
			bool allHaveJump = true;
			for (const auto& obj : strongSelectedObjs)
			{
				if (!obj->GetComponent<JumpBlockComponent>())
				{
					allHaveJump = false;
					break;
				}
			}
			if (allHaveJump)
			{
				DrawMultiEditJumpBlock(scene, m_selectedObjects);
			}

			//Transformと競合しないコンポーネントを追加
		}
	}

	ImGui::End();
}

void Editor::DrawSceneViewWindow(GameScene& scene)
{
	//ウィンドウの内側の余白(padding)を一時的に無くす
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

	if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoMove))
	{
		auto sceneTex = m_sceneRT.m_RTTexture;
		ImVec2 contentSize = ImGui::GetContentRegionAvail();
		if (sceneTex)
		{
			ImGui::Image(sceneTex->WorkSRView(), contentSize);
		}

		auto editorCameraComp = scene.GetEditorCamera();
		bool isAnyComponentGizmoUse = false;

		if (!m_selectedObjects.empty())
		{
			//================================================================
			// ギズモの描画と操作
			//================================================================
			if (editorCameraComp)
			{
				//ギズモ情報作成&収集
				auto camera = editorCameraComp->GetCamera();
				ImVec2 contentMin = ImGui::GetItemRectMin();
				const Math::Matrix& viewMat = camera->GetCameraViewMatrix();
				const Math::Matrix& projectionMat = camera->GetProjMatrix();

				//ギズモの共通設定
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
				ImGuizmo::SetRect(contentMin.x, contentMin.y, contentSize.x, contentSize.y);

				//単体ギズモ
				if (m_selectedObjects.size() == 1)
				{
					if (auto selectObject = m_selectedObjects[0].lock())
					{
						//コンテキストに情報セット
						EditorGizmoContext context;
						context.viewMat = &viewMat;
						context.projMat = &projectionMat;
						context.rectX = contentMin.x;
						context.rectY = contentMin.y;
						context.rectW = contentSize.x;
						context.rectH = contentSize.y;
						context.gizmoOperation = m_currentGizmoOperation;

						//各コンポーネントでギズモ描画
						for (const auto& component : selectObject->GetComponents())
						{
							if (component->OnDrawGizmos(context, scene))
							{
								isAnyComponentGizmoUse = true;
							}
						}
					}
				}
				//複数ギズモ
				else
				{
					//中心点(ピボット)計算
					Math::Vector3 centerPos = Math::Vector3::Zero;
					int validObjectCount = 0;

					for (const auto& weak_obj : m_selectedObjects)
					{
						if (auto obj = weak_obj.lock())
						{
							if (auto transform = obj->GetComponent<TransformComponent>())
							{
								centerPos += transform->GetPos();
								validObjectCount++;
							}
						}
					}

					if (validObjectCount > 0)
					{
						centerPos = centerPos / (float)validObjectCount;
					}

					if (!m_isGizmoDragging)
					{
						m_gizmoMat = Math::Matrix::CreateTranslation(centerPos);
					}

					if (ImGuizmo::IsUsing() && !m_isGizmoDragging)
					{
						m_beforeGizmoMat = m_gizmoMat;
						m_beforeObjsMat.clear();

						for (const auto& weak_obj : m_selectedObjects)
						{
							if (auto obj = weak_obj.lock())
							{
								if (auto transform = obj->GetComponent<TransformComponent>())
								{
									m_beforeObjsMat.push_back(transform->GetMatrix());
								}
							}
						}
						m_isGizmoDragging = true;
					}

					//ギズモの操作と描画
					ImGuizmo::Manipulate(
						(float*)&viewMat,
						(float*)&projectionMat,
						m_currentGizmoOperation,
						ImGuizmo::LOCAL,
						(float*)&m_gizmoMat
					);

					if (ImGuizmo::IsUsing())
					{
						Math::Matrix deltaMat = m_gizmoMat * m_beforeGizmoMat.Invert();

						//差分行列分解
						Math::Vector3 vDeltaPos, vDeltaScale;
						Math::Quaternion vDeltaQuat;
						deltaMat.Decompose(vDeltaScale, vDeltaQuat, vDeltaPos);

						if (m_beforeObjsMat.size() == m_selectedObjects.size())
						{
							for (size_t i = 0; i < m_selectedObjects.size(); ++i)
							{
								if (auto obj = m_selectedObjects[i].lock())
								{
									if (auto transform = obj->GetComponent<TransformComponent>())
									{
										Math::Vector3 originalPos, originalScale;
										Math::Quaternion originalQuat;
										m_beforeObjsMat[i].Decompose(originalScale, originalQuat, originalPos);

										Math::Vector3 newPos = originalPos;
										Math::Quaternion newQuat = originalQuat;
										Math::Vector3 newScale = originalScale;

										if (m_currentGizmoOperation == ImGuizmo::TRANSLATE)
										{
											newPos += vDeltaPos;
										}
										else if (m_currentGizmoOperation == ImGuizmo::ROTATE)
										{
											Math::Vector3 posRelativeToPivot = originalPos - centerPos;
											posRelativeToPivot = Math::Vector3::Transform(posRelativeToPivot, vDeltaQuat);
											newPos = posRelativeToPivot + centerPos;
											newQuat = vDeltaQuat * originalQuat;
										}
										else
										{
											newScale += (vDeltaScale - Math::Vector3::One);
										}

										//計算結果適用
										Math::Vector3 newRot = newQuat.ToEuler();
										transform->SetPos(newPos);
										transform->SetRot({
											DirectX::XMConvertToDegrees(newRot.x),
											DirectX::XMConvertToDegrees(newRot.y),
											DirectX::XMConvertToDegrees(newRot.z)
											});
										transform->SetScale(newScale);
									}
								}
							}
						}
					}
					else if (m_isGizmoDragging)
					{
						if (auto viewModel = scene.GetViewModel())
						{
							std::vector<std::shared_ptr<GameObject>> strongObjs;
							for (const auto& weak_obj : m_selectedObjects)
							{
								if (auto obj = weak_obj.lock())
								{
									strongObjs.push_back(obj);
								}
							}
							viewModel->UpdateStateFromGameObjects(strongObjs);
						}
						m_isGizmoDragging = false;
					}
				}
			}
		}

		//================================================================
		// オブジェクト選択 (レイキャスト)
		//================================================================
		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver() && !isAnyComponentGizmoUse)
		{
			if (editorCameraComp)
			{
				auto camera = editorCameraComp->GetCamera();
				auto cameraTransform = editorCameraComp->GetOwner()->GetComponent<TransformComponent>();
				if (cameraTransform && contentSize.x > 0 && contentSize.y > 0)
				{
					ImVec2 contentMin = ImGui::GetItemRectMin();
					ImVec2 mousePos = ImGui::GetMousePos();
					POINT finalMousePos = { static_cast<LONG>(mousePos.x - contentMin.x), static_cast<LONG>(mousePos.y - contentMin.y) };
					D3D11_VIEWPORT currentVP;
					camera->GetViewport(currentVP);
					camera->SetViewport(0, 0, contentSize.x, contentSize.y);
					camera->SetProjectionMatrix(60.0f, 2000.0f, 0.01f, contentSize.x / contentSize.y);
					camera->SetToShader();
					RayInfo ray;
					camera->GenerateRayInfoFromViewportPos(finalMousePos, ray.m_start, ray.m_dir, ray.m_maxDistance);
					camera->SetViewport(currentVP);
					camera->SetProjectionMatrix(60.0f);
					camera->UpdateViewMatrix();
					camera->SetToShader();

					bool isCtrlDown = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
					RayResult result;
					if (PhysicsSystem::Instance().Raycast(ray, result, LayerAll))
					{
						if (auto hitObj = result.m_hitObject.lock())
						{
							//Ctrlが押されていない場合
							if (!isCtrlDown)
							{
								m_selectedObjects.clear();
								m_selectedObjects.push_back(hitObj);
							}
							//Ctrlが押されている場合
							else
							{
								bool isAlreadySelected = false;
								for (const auto& weak_selected : m_selectedObjects)
								{
									if (weak_selected.lock() == hitObj)
									{
										isAlreadySelected = true;
										break;
									}
								}

								if (isAlreadySelected)
								{
									std::erase_if(m_selectedObjects, [&](const auto& weak_selected) {
										return weak_selected.lock() == hitObj;
										});
								}
								else
								{
									m_selectedObjects.push_back(hitObj);
								}
							}
						}
					}
					else
					{
						if (!isCtrlDown)
						{
							m_selectedObjects.clear();
						}
					}
				}
			}
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

void Editor::DrawSwapToolWindow(GameScene& scene)
{
	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.9f);

	// ウィンドウの位置とサイズを固定（初回のみ）
	// ここでは左下に表示する例
	ImGui::SetNextWindowPos(ImVec2(400, 550), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_FirstUseEver);

	ImGui::Begin("Swap Tool");

	//--スロットA--
	ImGui::SeparatorText("Slot A");
	ImGui::Text("Target: %s", m_swapSlotA.GetName().c_str());
	if (ImGui::Button("Set from Selected Object ##A"))
	{
		if (!m_selectedObjects.empty())
		{
			m_swapSlotA.type = SwapTarget::Type::Object;
			m_swapSlotA.object = m_selectedObjects[0];
			m_swapSlotA.group = nullptr;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Set from Selected Group ##A"))
	{
		if (m_selectedGroupIndex != -1)
		{
			m_swapSlotA.type = SwapTarget::Type::Group;
			m_swapSlotA.object.reset();
			m_swapSlotA.group = &m_groups[m_selectedGroupIndex];
		}
	}

	//--スロットB--
	ImGui::SeparatorText("Slot B");
	ImGui::Text("Target: %s", m_swapSlotB.GetName().c_str());
	if (ImGui::Button("Set from Selected Object ##B"))
	{
		if (!m_selectedObjects.empty())
		{
			m_swapSlotB.type = SwapTarget::Type::Object;
			m_swapSlotB.object = m_selectedObjects[0];
			m_swapSlotB.group = nullptr;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("Set from Selected Group ##B"))
	{
		if (m_selectedGroupIndex != -1)
		{
			m_swapSlotB.type = SwapTarget::Type::Group;
			m_swapSlotB.object.reset();
			m_swapSlotB.group = &m_groups[m_selectedGroupIndex];
		}
	}

	ImGui::Separator();

	//--実行--
	if (m_swapSlotA.type != SwapTarget::Type::None && m_swapSlotB.type != SwapTarget::Type::None)
	{
		if (ImGui::Button("Execute Swap", ImVec2(-1, 0)))
		{
			auto viewModel = scene.GetViewModel();

			//差分ベクトル計算
			Math::Vector3 pivotA = CalculateTargetPivot(m_swapSlotA, scene);
			Math::Vector3 pivotB = CalculateTargetPivot(m_swapSlotB, scene);
			Math::Vector3 delta = pivotB - pivotA;

			//ID取得
			std::vector<UINT> groupAIDs, groupBIDs;

			if (m_swapSlotA.type == SwapTarget::Type::Object)
			{
				if (auto obj = m_swapSlotA.object.lock())
				{
					if (auto idComp = obj->GetComponent<IdComponent>())
					{
						groupAIDs.push_back(idComp->GetId());
					}
				}
			}
			else if (m_swapSlotA.type == SwapTarget::Type::Group)
			{
				groupAIDs = m_swapSlotA.group->memberObjectIDs;
			}

			if (m_swapSlotB.type == SwapTarget::Type::Object)
			{
				if (auto obj = m_swapSlotB.object.lock())
				{
					if (auto idComp = obj->GetComponent<IdComponent>())
					{
						groupBIDs.push_back(idComp->GetId());
					}
				}
			}
			else if (m_swapSlotB.type == SwapTarget::Type::Group)
			{
				groupBIDs = m_swapSlotB.group->memberObjectIDs;
			}

			if (!groupAIDs.empty() && !groupBIDs.empty())
			{
				viewModel->RequestGroupSwap(groupAIDs, groupBIDs, delta);
			}

			m_swapSlotA.type = SwapTarget::Type::None;
			m_swapSlotA.object.reset();
			m_swapSlotA.group = nullptr;
			m_swapSlotB.type = SwapTarget::Type::None;
			m_swapSlotB.object.reset();
			m_swapSlotB.group = nullptr;
		}
	}
	//Execute受け付け不可
	else
	{
		ImGui::BeginDisabled();
		ImGui::Button("Execute Swap", ImVec2(-1, 0));
		ImGui::EndDisabled();
	}

	if (ImGui::Button("Clear Sloats", ImVec2(-1, 0)))
	{
		m_swapSlotA.type = SwapTarget::Type::None;
		m_swapSlotA.object.reset();
		m_swapSlotA.group = nullptr;
		m_swapSlotB.type = SwapTarget::Type::None;
		m_swapSlotB.object.reset();
		m_swapSlotB.group = nullptr;
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

void Editor::DrawMultiEditTransform(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj)
{
	auto viewModel = scene.GetViewModel();
	if (!viewModel)return;

	std::vector<std::shared_ptr<GameObject>> strongSelectedObjects;

	for (auto& weak_obj : selectedObj)
	{
		if (auto obj = weak_obj.lock())
		{
			strongSelectedObjects.push_back(obj);
		}
	}
	if (strongSelectedObjects.empty())return;
	
	auto firstObj = strongSelectedObjects[0];
	if (!firstObj->GetComponent<TransformComponent>())return;

	//閉じている間は描画を行わない
	if (!ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	static float pos[3], rot[3], scale[3];

	ImGui::PushID("MultiTransformEdit");//selection_hashというIDをローカル化させるため

	//選択中のオブジェクトのメモリアドレスを使ってハッシュ値をユニーク化
	uintptr_t selectionHash = 0;
	for (const auto& obj : strongSelectedObjects)
	{
		//ポインタのアドレスを数値として足し合わせる
		selectionHash += reinterpret_cast<uintptr_t>(obj.get());
	}
	
	//計算したハッシュ値が以前と異なっていれば初期化
	if (ImGui::GetStateStorage()->GetVoidPtr(ImGui::GetID("selection_hash")) != (void*)selectionHash)
	{
		Math::Vector3 firstPos = firstObj->GetComponent<TransformComponent>()->GetPos();
		pos[0] = firstPos.x;
		pos[1] = firstPos.y;
		pos[2] = firstPos.z;

		Math::Vector3 firstRot = firstObj->GetComponent<TransformComponent>()->GetRot();
		rot[0] = firstRot.x;
		rot[1] = firstRot.y;
		rot[2] = firstRot.z;

		Math::Vector3 firstScale = firstObj->GetComponent<TransformComponent>()->GetScale();
		scale[0] = firstScale.x;
		scale[1] = firstScale.y;
		scale[2] = firstScale.z;

		ImGui::GetStateStorage()->SetVoidPtr(ImGui::GetID("selection_hash"), (void*)selectionHash);
	}

	ImGui::PopID();

	//--Position--
	//ToDo: mixedX, Y, Zを使って、値が混在している場合にUIの見た目を変える（例：文字色をグレーにするなど）
	ImGui::Text("Position");
	ImGui::DragFloat3("##Position", pos, 0.1f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		//選択中のオブジェクトに変更を適用(上のDragFloatでは何も変更してないから)
		for (auto& obj : strongSelectedObjects)
		{
			if (auto transform = obj->GetComponent<TransformComponent>())
			{
				transform->SetPos({ pos[0], pos[1], pos[2] });
			}
		}

		viewModel->UpdateStateFromGameObjects(strongSelectedObjects);
	}

	//--Rotation--
	ImGui::Text("Rotation");
	ImGui::DragFloat3("##Rotation", rot, 1.0f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjects)
		{
			if (auto transform = obj->GetComponent<TransformComponent>())
			{
				transform->SetRot({ rot[0], rot[1], rot[2] });
			}
		}

		viewModel->UpdateStateFromGameObjects(strongSelectedObjects);
	}

	//--Scale--
	ImGui::Text("Scale");
	ImGui::DragFloat3("##Scale", scale, 0.01f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjects)
		{
			if (auto transform = obj->GetComponent<TransformComponent>())
			{
				transform->SetScale({ scale[0], scale[1], scale[2] });
			}
		}

		viewModel->UpdateStateFromGameObjects(strongSelectedObjects);
	}
}

void Editor::DrawMultiEditMovingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj)
{
	auto viewModel = scene.GetViewModel();
	if (!viewModel)return;

	std::vector<std::shared_ptr<GameObject>> strongSelectedObjs;
	for (auto& weak_obj : selectedObj)
	{
		if (auto obj = weak_obj.lock())
		{
			strongSelectedObjs.push_back(obj);
		}
	}

	if (strongSelectedObjs.empty())return;

	auto firstMovingBlock = strongSelectedObjs[0]->GetComponent<MovingBlockComponent>();
	if (!firstMovingBlock)return;

	if (!ImGui::CollapsingHeader("Moving Block Component(Multi-Edit)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	//--往復速度--
	static float duration;

	ImGui::PushID("MultiMovingBlockEdit");
	
	uintptr_t selectionHash = 0;
	for (const auto& obj : strongSelectedObjs)
	{
		//ポインタのアドレスを数値として足し合わせる
		selectionHash += reinterpret_cast<uintptr_t>(obj.get());
	}

	if (ImGui::GetStateStorage()->GetVoidPtr(ImGui::GetID("selection_hash")) != (void*)selectionHash)
	{
		duration = firstMovingBlock->GetDuration();

		ImGui::GetStateStorage()->SetVoidPtr(ImGui::GetID("selection_hash"), (void*)selectionHash);
	}
	ImGui::PopID();

	ImGui::Text("Duration");
	ImGui::DragFloat("Duration(seconds)", &duration, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto movingBlock = obj->GetComponent<MovingBlockComponent>())
			{
				movingBlock->SetDuration(duration);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	ImGui::Text("Start/End Positions are edited individually.");
}

void Editor::DrawMultiEditSinkingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj)
{
	auto viewModel = scene.GetViewModel();
	if (!viewModel)return;

	std::vector<std::shared_ptr<GameObject>> strongSelectedObjs;
	for (auto& weak_obj : selectedObj)
	{
		if (auto obj = weak_obj.lock())
		{
			strongSelectedObjs.push_back(obj);
		}
	}

	if (strongSelectedObjs.empty())return;

	auto firstsinkingBlock = strongSelectedObjs[0]->GetComponent<SinkingBlockComponent>();
	if (!firstsinkingBlock)return;

	if (!ImGui::CollapsingHeader("Sinking Block Component(Multi-Edit)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	static float sinkDistance, acceleration, riseSpeed;

	ImGui::PushID("MultiSinkingBlockEdit");

	uintptr_t selectionHash = 0;
	for (const auto& obj : strongSelectedObjs)
	{
		//ポインタのアドレスを数値として足し合わせる
		selectionHash += reinterpret_cast<uintptr_t>(obj.get());
	}

	if (ImGui::GetStateStorage()->GetVoidPtr(ImGui::GetID("selection_hash")) != (void*)selectionHash)
	{
		sinkDistance = firstsinkingBlock->GetMaxSinkDistance();
		acceleration = firstsinkingBlock->GetAcceleration();
		riseSpeed = firstsinkingBlock->GetRiseSpeed();

		ImGui::GetStateStorage()->SetVoidPtr(ImGui::GetID("selection_hash"), (void*)selectionHash);
	}
	ImGui::PopID();

	//--沈降量--
	ImGui::Text("Max Sink Distance");
	ImGui::DragFloat("Max Sink Distance", &sinkDistance, 0.1f, 1.0f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto sinkingBlock = obj->GetComponent<SinkingBlockComponent>())
			{
				sinkingBlock->SetMaxSinkDistance(sinkDistance);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	//--沈降速度--
	ImGui::Text("Acceleration");
	ImGui::DragFloat("Acceleration", &acceleration, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto sinkingBlock = obj->GetComponent<SinkingBlockComponent>())
			{
				sinkingBlock->SetAcceleration(acceleration);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	//--上昇スピード--
	ImGui::Text("Rise Speed");
	ImGui::DragFloat("Rise Speed", &riseSpeed, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto sinkingBlock = obj->GetComponent<SinkingBlockComponent>())
			{
				sinkingBlock->SetRiseSpeed(riseSpeed);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	ImGui::Text("Initial Positions are edited individually.");
}

void Editor::DrawMultiEditScalingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj)
{
	auto viewModel = scene.GetViewModel();
	if (!viewModel)return;

	std::vector<std::shared_ptr<GameObject>> strongSelectedObjs;
	for (auto& weak_obj : selectedObj)
	{
		if (auto obj = weak_obj.lock())
		{
			strongSelectedObjs.push_back(obj);
		}
	}

	if (strongSelectedObjs.empty())return;

	auto firstScalingBlock = strongSelectedObjs[0]->GetComponent<ScalingBlockComponent>();
	if (!firstScalingBlock)return;

	if (!ImGui::CollapsingHeader("Scaling Block Component(Multi-Edit)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	static float scaleAmount, scaleSpeed, scaleAxis[3];

	ImGui::PushID("MultiScalingBlockEdit");

	uintptr_t selectionHash = 0;
	for (const auto& obj : strongSelectedObjs)
	{
		//ポインタのアドレスを数値として足し合わせる
		selectionHash += reinterpret_cast<uintptr_t>(obj.get());
	}

	if (ImGui::GetStateStorage()->GetVoidPtr(ImGui::GetID("selection_hash")) != (void*)selectionHash)
	{
		Math::Vector3 firstScale = firstScalingBlock->GetScaleAxis();
		scaleAxis[0] = firstScale.x;
		scaleAxis[1] = firstScale.y;
		scaleAxis[2] = firstScale.z;

		scaleAmount = firstScalingBlock->GetscaleAmount();

		scaleSpeed = firstScalingBlock->GetscaleSpeed();
		

		ImGui::GetStateStorage()->SetVoidPtr(ImGui::GetID("selection_hash"), (void*)selectionHash);
	}
	ImGui::PopID();

	//--拡縮軸--
	ImGui::Text("Scaling Axis");
	ImGui::DragFloat3("Scaling Axis", scaleAxis, 1.0f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto scaling = obj->GetComponent<ScalingBlockComponent>())
			{
				scaling->SetScaleAxis({ scaleAxis[0],scaleAxis[1] ,scaleAxis[2] });
			}
		}

		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	//--拡縮量--
	ImGui::Text("Scale Amount");
	ImGui::DragFloat("Scale Amount", &scaleAmount, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto scalingBlock = obj->GetComponent<ScalingBlockComponent>())
			{
				scalingBlock->SetScaleAmount(scaleAmount);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	//--拡縮スピード--
	ImGui::Text("Scale Speed");
	ImGui::DragFloat("Scale Speed", &scaleSpeed, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto scalingBlock = obj->GetComponent<ScalingBlockComponent>())
			{
				scalingBlock->SetScaleSpeed(scaleSpeed);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}
}

void Editor::DrawMultiEditRotatingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj)
{
	auto viewModel = scene.GetViewModel();
	if (!viewModel)return;

	std::vector<std::shared_ptr<GameObject>> strongSelectedObjs;
	for (auto& weak_obj : selectedObj)
	{
		if (auto obj = weak_obj.lock())
		{
			strongSelectedObjs.push_back(obj);
		}
	}

	if (strongSelectedObjs.empty())return;

	auto firstRotatingBlock = strongSelectedObjs[0]->GetComponent<RotatingBlockComponent>();
	if (!firstRotatingBlock)return;

	if (!ImGui::CollapsingHeader("Rotating Block Component(Multi-Edit)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	static float rotAmount, rotSpeed, rotAxis[3];

	ImGui::PushID("MultiRotatingBlockEdit");

	uintptr_t selectionHash = 0;
	for (const auto& obj : strongSelectedObjs)
	{
		//ポインタのアドレスを数値として足し合わせる
		selectionHash += reinterpret_cast<uintptr_t>(obj.get());
	}

	if (ImGui::GetStateStorage()->GetVoidPtr(ImGui::GetID("selection_hash")) != (void*)selectionHash)
	{
		Math::Vector3 firstRot = firstRotatingBlock->GetRotatingAxis();
		rotAxis[0] = firstRot.x;
		rotAxis[1] = firstRot.y;
		rotAxis[2] = firstRot.z;

		rotAmount = firstRotatingBlock->GetRotatingAmount();

		rotSpeed = firstRotatingBlock->GetRotatingSpeed();


		ImGui::GetStateStorage()->SetVoidPtr(ImGui::GetID("selection_hash"), (void*)selectionHash);
	}
	ImGui::PopID();

	//--回転軸--
	ImGui::Text("Rotating Axis");
	ImGui::DragFloat3("Rotating Axis", rotAxis, 1.0f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto rotating = obj->GetComponent<RotatingBlockComponent>())
			{
				rotating->SetRotatingAxis({ rotAxis[0],rotAxis[1] ,rotAxis[2] });
			}
		}

		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	//--回転量--
	ImGui::Text("Rotation Amount");
	ImGui::DragFloat("Rotation Amount", &rotAmount, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto rotatingBlock = obj->GetComponent<RotatingBlockComponent>())
			{
				rotatingBlock->SetRotatingAmount(rotAmount);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	//--回転スピード--
	ImGui::Text("Rotation Speed");
	ImGui::DragFloat("Rotation Speed", &rotSpeed, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto rotatingBlock = obj->GetComponent<RotatingBlockComponent>())
			{
				rotatingBlock->SetRotatingSpeed(rotSpeed);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}
}

void Editor::DrawMultiEditSlipperyBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj)
{
	auto viewModel = scene.GetViewModel();
	if (!viewModel)return;

	std::vector<std::shared_ptr<GameObject>> strongSelectedObjs;
	for (auto& weak_obj : selectedObj)
	{
		if (auto obj = weak_obj.lock())
		{
			strongSelectedObjs.push_back(obj);
		}
	}

	if (strongSelectedObjs.empty())return;

	auto firstSlipperyBlock = strongSelectedObjs[0]->GetComponent<SlipperyComponent>();
	if (!firstSlipperyBlock)return;

	if (!ImGui::CollapsingHeader("Slippery Block Component(Multi-Edit)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	//--摩擦係数--
	static float dragCoefficient;

	ImGui::PushID("MultiSlipperyBlockEdit");

	uintptr_t selectionHash = 0;
	for (const auto& obj : strongSelectedObjs)
	{
		//ポインタのアドレスを数値として足し合わせる
		selectionHash += reinterpret_cast<uintptr_t>(obj.get());
	}

	if (ImGui::GetStateStorage()->GetVoidPtr(ImGui::GetID("selection_hash")) != (void*)selectionHash)
	{
		dragCoefficient = firstSlipperyBlock->GetDragCoefficient();

		ImGui::GetStateStorage()->SetVoidPtr(ImGui::GetID("selection_hash"), (void*)selectionHash);
	}
	ImGui::PopID();

	ImGui::Text("Drag Coefficient");
	ImGui::DragFloat("Drag Coefficient", &dragCoefficient, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto slipperyBlock = obj->GetComponent<SlipperyComponent>())
			{
				slipperyBlock->SetDragCoefficient(dragCoefficient);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}
}

void Editor::DrawMultiEditJumpBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj)
{
	auto viewModel = scene.GetViewModel();
	if (!viewModel)return;

	std::vector<std::shared_ptr<GameObject>> strongSelectedObjs;
	for (auto& weak_obj : selectedObj)
	{
		if (auto obj = weak_obj.lock())
		{
			strongSelectedObjs.push_back(obj);
		}
	}

	if (strongSelectedObjs.empty())return;

	auto firstJumpBlock = strongSelectedObjs[0]->GetComponent<JumpBlockComponent>();
	if (!firstJumpBlock)return;

	if (!ImGui::CollapsingHeader("Jump Block Component(Multi-Edit)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	static float jumpForce, jumpDirection[3];

	ImGui::PushID("MultiJumpBlockEdit");

	uintptr_t selectionHash = 0;
	for (const auto& obj : strongSelectedObjs)
	{
		//ポインタのアドレスを数値として足し合わせる
		selectionHash += reinterpret_cast<uintptr_t>(obj.get());
	}

	if (ImGui::GetStateStorage()->GetVoidPtr(ImGui::GetID("selection_hash")) != (void*)selectionHash)
	{
		Math::Vector3 dir = firstJumpBlock->GetJumpDirection();
		jumpDirection[0] = dir.x;
		jumpDirection[1] = dir.y;
		jumpDirection[2] = dir.z;

		jumpForce = firstJumpBlock->GetJumpForce();

		ImGui::GetStateStorage()->SetVoidPtr(ImGui::GetID("selection_hash"), (void*)selectionHash);
	}
	ImGui::PopID();

	//--ジャンプ方向--
	ImGui::Text("Jump Direction");
	ImGui::DragFloat3("Jump Direction", jumpDirection, 0.01f);

	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto jump = obj->GetComponent<JumpBlockComponent>())
			{
				jump->SetJumpDirection({ jumpDirection[0],jumpDirection[1] ,jumpDirection[2] });
			}
		}

		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}

	//--ジャンプ力--
	ImGui::Text("Jump Force");
	ImGui::DragFloat("Jump Force", &jumpForce, 0.1f, 0.1f);
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		for (auto& obj : strongSelectedObjs)
		{
			if (auto jumpBlock = obj->GetComponent<JumpBlockComponent>())
			{
				jumpBlock->SetJumpForce(jumpForce);
			}
		}
		viewModel->UpdateStateFromGameObjects(strongSelectedObjs);
	}
}

void Editor::DrawMultiGizmoTransform()
{
	if (!ImGui::CollapsingHeader("Gizmo Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		return;
	}

	//m_gizmoMatから分解
	Math::Vector3 scale, pos;
	Math::Quaternion rotQuat;
	m_gizmoMat.Decompose(scale, rotQuat, pos);

	Math::Vector3 rotEuler = rotQuat.ToEuler();
	rotEuler.x = DirectX::XMConvertToDegrees(rotEuler.x);
	rotEuler.y = DirectX::XMConvertToDegrees(rotEuler.y);
	rotEuler.z = DirectX::XMConvertToDegrees(rotEuler.z);

	ImGui::Text("Position");
	ImGui::InputFloat3("##GizmoPosition", &pos.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Rotation (Euler)");
	ImGui::InputFloat3("##GizmoRotation", &rotEuler.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	ImGui::Text("Scale");
	ImGui::InputFloat3("##GizmoScale", &scale.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

Math::Vector3 Editor::CalculateTargetPivot(const Editor::SwapTarget& target, const GameScene& scene)
{
	Math::Vector3 pivot = Math::Vector3::Zero;
	int count = 0;

	if (target.type == SwapTarget::Type::Object)
	{
		if (auto obj = target.object.lock())
		{
			if (auto transform = obj->GetComponent<TransformComponent>())
			{
				pivot += transform->GetPos();
				count = 1;
			}
		}
	}
	else if (target.type == SwapTarget::Type::Group)
	{
		for (UINT id : target.group->memberObjectIDs)
		{
			for (const auto& obj : scene.GetObjList())
			{
				if (auto idComp = obj->GetComponent<IdComponent>())
				{
					if (id == idComp->GetId())
					{
						if (auto transform = obj->GetComponent<TransformComponent>())
						{
							pivot += transform->GetPos();
							count++;
						}
						break;
					}
				}
			}
		}
	}

	if (count > 0)
	{
		pivot /= (float)count;
	}

	return pivot;
}
