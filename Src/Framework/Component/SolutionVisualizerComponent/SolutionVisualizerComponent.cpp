#include "SolutionVisualizerComponent.h"
#include"../Src/Application/GameViewModel.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"
#include"../ColliderComponent/ColliderComponent.h"
#include"../TransformComponent/TransformComponent.h"
#include"../GameObject.h"
#include"../../Command/SwapBlockCommand/SwapBlockCommand.h"
#include"../Src/Application/GameLogic/StageModel/StageModel.h"

void SolutionVisualizerComponent::Awake()
{
	m_spArrowModel[0] = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Arrow/Arrow4.gltf");
	m_spArrowModel[1] = KdAssets::Instance().m_modeldatas.GetData("Asset/Models/Arrow/Arrow5.gltf");
}

void SolutionVisualizerComponent::Update()
{
	if (!m_isPlaying)return;
	auto viewModel = m_wpViewModel.lock();
	if (!viewModel)
	{
		m_isPlaying = false;
		return;
	}

	const auto& steps = viewModel->GetSolutionSteps();
	if (steps.empty() || m_currentStepIndex >= steps.size())
	{
		Reset();
		return;
	}

	m_animationProgress += Application::Instance().GetDeltaTime() * m_animationSpeed;

	//1つのペアのアニメーションが終わったら
	if (m_animationProgress >= 1.0f)
	{
		const auto& step = steps[m_currentStepIndex];
		auto tempCommand = std::make_unique<SwapBlockCommand>(viewModel->GetModel(), step.fromID, step.toID, true);
		tempCommand->Execute();

		m_animationProgress = 0.0f;
		m_currentStepIndex++;
	}
}

void SolutionVisualizerComponent::DrawLit()
{
	if (!m_bShouldDraw|| !m_spArrowModel)return;
	auto viewModel = m_wpViewModel.lock();
	if (!viewModel)return;

	const auto& steps = viewModel->GetSolutionSteps();
	if (steps.empty())return;

	if (m_viewMode == SolutionViewMode::Static)
	{
		const auto& startState = viewModel->GetSolutionStartState();
		if (startState.empty())
		{
			//KdShaderManager::Instance().UndoDepthStencilState();
			return;
		}

		for (const auto& step : steps)
		{
			if (startState.count(step.fromID) && startState.count(step.toID))
			{
				const BlockState& fromState = startState.at(step.fromID);
				const BlockState& toState = startState.at(step.toID);

				Math::Matrix arrowMat = CalculateArrowMatrix(fromState.pos, toState.pos);

				KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZDisable);
				KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spArrowModel[1], arrowMat);
				KdShaderManager::Instance().UndoDepthStencilState();
			}
		}
	}
	else
	{
		if (m_isPlaying || m_animationProgress > 0.0f)
		{
			if (m_currentStepIndex >= steps.size())return;

			const auto& currentStep = steps[m_currentStepIndex];
			auto model = viewModel->GetModel();

			//現在のブロックの状態を取得
			const BlockState* fromState = model->GetBlockState(currentStep.fromID);
			const BlockState* toState = model->GetBlockState(currentStep.toID);

			if (fromState && toState)
			{
				// from->toのベクトル計算
				Math::Vector3 fromToDir = toState->pos - fromState->pos;
				Math::Vector3 fromTo = Math::Vector3::Lerp(fromState->pos, toState->pos, m_animationProgress);

				// to->fromのベクトル計算
				Math::Vector3 toFromDir = fromState->pos - toState->pos;
				Math::Vector3 toFrom = Math::Vector3::Lerp(toState->pos, fromState->pos, m_animationProgress);

				// 描画
				Math::Matrix arrowMatA = CalculateArrowMatrixAnimation(fromTo, fromToDir);
				KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZDisable);
				KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spArrowModel[0], arrowMatA);
				KdShaderManager::Instance().UndoDepthStencilState();

				Math::Matrix arrowMatB = CalculateArrowMatrixAnimation(toFrom, toFromDir);
				KdShaderManager::Instance().ChangeDepthStencilState(KdDepthStencilState::ZDisable);
				KdShaderManager::Instance().m_StandardShader.DrawModel(*m_spArrowModel[0], arrowMatB);
				KdShaderManager::Instance().UndoDepthStencilState();
			}
		}
	}
}

void SolutionVisualizerComponent::SetShouldDraw(bool flg)
{
	if (m_bShouldDraw == flg)return;

	m_bShouldDraw = flg;

	if (m_bShouldDraw)
	{
		EnterVisualizationMode();
	}
	else
	{
		ExitVisualizationMode();
	}
}

void SolutionVisualizerComponent::Play()
{
	auto viewModel = m_wpViewModel.lock();
	if (!viewModel)return;
	if (m_isPlaying)return;

	if (m_currentStepIndex >= viewModel->GetSolutionSteps().size())
	{
		Reset();
	}

	EnterVisualizationMode();

	m_isPlaying = true;
}

void SolutionVisualizerComponent::Pause()
{
	m_isPlaying = false;
}

void SolutionVisualizerComponent::Reset()
{
	m_isPlaying = false;
	m_currentStepIndex = 0;
	m_animationProgress = 0.0f;

	ExitVisualizationMode();
}

Math::Matrix SolutionVisualizerComponent::CalculateArrowMatrix(const Math::Vector3& from, const Math::Vector3& to)
{
	Math::Vector3 dir = to - from;
	float length = dir.Length();
	if (length > 0.001f)
	{
		dir.Normalize();
	}
	else
	{
		return Math::Matrix::Identity;
	}

	Math::Vector3 pos = (from + to) / 2.0f;

	//Z軸正面をデフォルト値とする
	Math::Vector3 initialDir = Math::Vector3::UnitZ;

	//回転行列作成
	Math::Quaternion rot = Math::Quaternion::FromToRotation(initialDir, dir);
	Math::Matrix rotMat = Math::Matrix::CreateFromQuaternion(rot);

	//拡縮行列作成
	float showLength = length / 6.0f;
	Math::Matrix scaleMat = Math::Matrix::CreateScale(1.0f, 1.0f, showLength);

	//座標行列作成
	Math::Matrix transMat = Math::Matrix::CreateTranslation(pos);

	//最終的な行列作成
	Math::Matrix finalMat = scaleMat * rotMat * transMat;

	return finalMat;
}

Math::Matrix SolutionVisualizerComponent::CalculateArrowMatrixAnimation(const Math::Vector3& pos, const Math::Vector3& dir)
{
	Math::Vector3 direction = dir;
	if (direction.LengthSquared() < 0.001f)//向きがほぼゼロのとき
	{
		direction = Math::Vector3::UnitZ;//デフォルト値
	}

	//回転行列作成
	Math::Vector3 initialDir = Math::Vector3::UnitZ;
	Math::Quaternion rot = Math::Quaternion::FromToRotation(initialDir, direction);
	Math::Matrix rotMat = Math::Matrix::CreateFromQuaternion(rot);

	//拡縮行列作成
	Math::Matrix scaleMat = Math::Matrix::CreateScale(1.0f, 1.0f, 1.0f);

	//座標行列作成
	Math::Vector3 arrowPos = pos;
	arrowPos.y += 0.6f;
	Math::Matrix transMat = Math::Matrix::CreateTranslation(arrowPos);

	//最終的な行列作成
	Math::Matrix finalMat = scaleMat * rotMat * transMat;

	return finalMat;
}

void SolutionVisualizerComponent::EnterVisualizationMode()
{
	auto viewModel = m_wpViewModel.lock();
	if (!viewModel)return;

	if (m_editorStateBackup.empty())
	{
		const auto& allStates = viewModel->GetModel()->GetAllBlockState();
		for (const auto& [id, state] : allStates)
		{
			m_editorStateBackup[id] = state;
		}

		//ステージを解法記録前の状態に戻す
		viewModel->ForceSetStageState(viewModel->GetSolutionStartState());
	}
}

void SolutionVisualizerComponent::ExitVisualizationMode()
{
	//バックアップから復元
	if (!m_editorStateBackup.empty())
	{
		if (auto viewModel = m_wpViewModel.lock())
		{
			viewModel->ForceSetStageState(m_editorStateBackup);
			Application::Instance().AddLog("Editor state restored.");
		}
		m_editorStateBackup.clear();
	}
}
