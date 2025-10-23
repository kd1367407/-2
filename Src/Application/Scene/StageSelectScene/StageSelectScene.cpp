#include "StageSelectScene.h"
#include"../SceneManager.h"
#include"../GameScene/GameManager/GameManager.h"
#include"../../JsonHelper/JsonHelper.h"
#include"../GameScene/GameScene.h"
#include"../../GameLogic/StageModel/StageModel.h"
#include"../../GameViewModel.h"
#include"../Src/Framework/Component/TransformComponent/TransformComponent.h"
#include"../Src/Framework/Component/CameraComponent/CameraComponent.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/BackgroundComponent/BackgroundComponent.h"
#include"../../main.h"

using json = nlohmann::json;

void StageSelectScene::Init()
{
	SceneManager::Instance().SetMode(SceneManager::SceneMode::UI);
	m_currentView = ViewMode::Main;

	//背景
	auto backgroundObj = std::make_shared<GameObject>();
	backgroundObj->SetName("Background");
	backgroundObj->AddComponent<BackgroundComponent>();
	AddObject(backgroundObj);
	backgroundObj->Init();

	m_spBGM = KdAudioManager::Instance().Play("Asset/Sound/TitleBGM.wav", true, 1.0f);
}

void StageSelectScene::SceneUpdate()
{
	float deltatime = Application::Instance().GetDeltaTime();
	float fadeSpeed = 1.0f;
	auto& fader = SceneManager::Instance().GetFader();

	if (m_buttonAlpha < 1.0f)
	{
		if (!fader.IsFadeing())
		{
			m_buttonAlpha += fadeSpeed * deltatime;
		}
	}

	m_buttonAlpha = std::min(m_buttonAlpha, 1.0f);
}

void StageSelectScene::Draw()
{
	BaseScene::Draw();

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_buttonAlpha);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.2f, 0.4f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.5f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.4f, 0.6f, 1.0f));

	ImGui::SetNextWindowSize(ImVec2(400, 0));
	ImGui::Begin("Stage Select", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration);

	if (m_currentView == ViewMode::Main)
	{
		ImGui::BeginChild("MainStagesScroll", ImVec2(0, 300), true);
		DrawStageButtons("Asset/Data/Stages/StageList.json");
		ImGui::EndChild();

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (GameManager::Instance().GetLoadMode() == GameManager::LoadMode::Play)
		{
			if (ImGui::Button("Tutorials", ImVec2(-1, 0)))
			{
				m_currentView = ViewMode::Tutorial;
				KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
			}
		}
		////後で絶対に戻す
		//if (ImGui::Button("Tutorials", ImVec2(-1, 0)))
		//{
		//	m_currentView = ViewMode::Tutorial;
		//	KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
		//}

		if (ImGui::Button("Back To Title", ImVec2(-1, 0)))
		{
			SceneManager::Instance().ChangeScene(SceneManager::SceneType::Title);
		}
	}
	else
	{
		//ImGui::SeparatorText("Tutorial Stages");
		DrawStageButtons("Asset/Data/Stages/TutorialList.json");

		
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Back", ImVec2(-1, 0)))
		{
			m_currentView = ViewMode::Main;
			KdAudioManager::Instance().Play("Asset/Sound/UIButton.wav", false, 1.0f);
		}
	}

	ImGui::End();

	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar();
}

void StageSelectScene::Release()
{
	if (m_spBGM && m_spBGM->IsPlaying())
	{
		m_spBGM->Stop();
	}
	m_spBGM = nullptr;
}

void StageSelectScene::DrawStageButtons(const std::string& filePath)
{
	std::ifstream ifs(filePath);
	if (!ifs.is_open())return;

	json stageListData;
	ifs >> stageListData;

	if (stageListData.contains("stages") && stageListData["stages"].is_array())
	{
		for (const auto& stageInfo : stageListData["stages"])
		{
			std::string label = JsonHelper::GetString(stageInfo, "label", "Unknown Stage");
			std::string path = JsonHelper::GetString(stageInfo, "path");

			if (ImGui::Button(label.c_str(), ImVec2(-1, 0)) && !path.empty())
			{
				auto& gm = GameManager::Instance();
				gm.SetNextStage(path, label);

				//LoadModeに応じてsceneのモード決定
				if (gm.GetLoadMode() == GameManager::LoadMode::Play)
				{
					SceneManager::Instance().SetMode(SceneManager::SceneMode::Game);
				}
				else
				{
					SceneManager::Instance().SetMode(SceneManager::SceneMode::Create);
				}
				SceneManager::Instance().ChangeScene(SceneManager::SceneType::Game);
			}

			//マウスオーバーされているか
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			{

				GenerateStagePreview(path);

				ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.05f, 0.05f, 0.15f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.8f, 0.9f, 1.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 5.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

				//吹き出しでプレビュー表示
				ImGui::BeginTooltip();
				bool isHoverBegan = ImGui::IsWindowAppearing();

				//pathの欄があるか、中身があるか
				if (m_stagePreviews.count(path) && m_stagePreviews[path])
				{
					//SRVからID3D11ShaderResourceView*を取得
					ImTextureID texId = m_stagePreviews[path]->WorkSRView();
					ImGui::Image(texId, ImVec2(320, 180));
					if (isHoverBegan)
					{
						KdAudioManager::Instance().Play("Asset/Sound/Popup.wav", false, 1.0f);
					}
				}
				else
				{
					ImGui::Text("Preview loading...");
				}
				ImGui::EndTooltip();

				ImGui::PopStyleVar(2);
				ImGui::PopStyleColor(2);
			}
		}
	}
}

void StageSelectScene::GenerateStagePreview(const std::string& stagePath)
{
	if (m_stagePreviews.count(stagePath))return;

	//レンダーターゲット作成
	const int previewWidth = 320;
	const int previewHeight = 180;
	KdRenderTargetPack previewRT;
	previewRT.CreateRenderTarget(previewWidth, previewHeight, true);


	if (!previewRT.m_RTTexture)
	{
		m_stagePreviews[stagePath] = nullptr; // 失敗したことを記録
		return;
	}

	//一時的にGameSceneとViewModelを生成
	auto tempModel = std::make_shared<StageModel>();
	tempModel->Init();
	auto tempScene = std::make_unique<GameScene>();
	auto tempViewModel = std::make_shared<GameViewModel>(tempModel, nullptr, tempScene.get());
	tempViewModel->LoadStage(stagePath);

	//コンポーネントの状態を確定させる
	tempScene->PreUpdate();
	tempScene->Update();
	tempScene->PostUpdate();

	//プレビューカメラが設定されてない場合のカメラ行列
	Math::Matrix cameraMat = Math::Matrix::CreateLookAt(
		Math::Vector3(10.0f, 10.0f, -15.0f),//カメラの位置
		Math::Vector3::Zero,//見つめる先の座標（原点）
		Math::Vector3::Up//カメラの上方向
	).Invert();

	//PreviewCameraObjectを探す
	auto previewPoint = tempScene->FindObject("PreviewCamera");
	if (previewPoint)
	{
		if (auto pointTransform = previewPoint->GetComponent<TransformComponent>())
		{
			cameraMat = pointTransform->GetMatrix();
		}
	}

	//プレビューカメラ生成、シーンに追加
	auto cameraObj = std::make_shared<GameObject>();
	auto cameraComp = cameraObj->AddComponent<CameraComponent>();
	cameraObj->AddComponent<TransformComponent>();
	tempScene->AddObject(cameraObj);
	cameraObj->Init();
	cameraComp->GetCamera()->SetCameraMatrix(cameraMat);
	tempScene->SetActiveCamera(cameraComp);

	//レンダーターゲット切り替え、1フレーム描画
	{
		KdRenderTargetChanger rtChanger;
		rtChanger.ChangeRenderTarget(previewRT);
		previewRT.ClearTexture(Math::Color(1.0f, 0.0f, 1.0f, 1.0f)); //背景色を設定
		tempScene->PreDraw();
		tempScene->Draw();
	}

	//レンダーターゲットをテクスチャにして保存
	m_stagePreviews[stagePath] = previewRT.m_RTTexture;

	//ObjectListを解放するため
	tempScene->Release();
}
