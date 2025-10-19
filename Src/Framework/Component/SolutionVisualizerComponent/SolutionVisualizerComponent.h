#pragma once
#include"../Component.h"
#include"SolutionVisualizerTypes/SolutionVisualizerTypes.h"

class GameViewModel;
struct BlockState;

class SolutionVisualizerComponent:public Component
{
public:
	void Awake()override;
	void Update()override;
	void DrawLit()override;
	const char* GetComponentName() const override { return "SolutionVisualizerComponent"; }

	void SetViewModel(const std::weak_ptr<GameViewModel>& wp) { m_wpViewModel = wp; }
	void SetShouldDraw(bool flg);
	void SetViewMode(SolutionViewMode mode) { m_viewMode = mode; }

	void Play();
	void Pause();
	void Reset();

private:
	Math::Matrix CalculateArrowMatrix(const Math::Vector3& from, const Math::Vector3& to);
	Math::Matrix CalculateArrowMatrixAnimation(const Math::Vector3& pos, const Math::Vector3& dir);
	
	//バックアップと復元
	void EnterVisualizationMode();
	//エディタの操作盤面に戻す
	void ExitVisualizationMode();


	std::weak_ptr<GameViewModel> m_wpViewModel;
	std::shared_ptr<KdModelData> m_spArrowModel[2];
	bool m_bShouldDraw = false;
	std::map<UINT, BlockState> m_editorStateBackup;

	SolutionViewMode m_viewMode = SolutionViewMode::Static;
	bool m_isPlaying = false;
	int m_currentStepIndex = 0;
	float m_animationProgress = 0.0f;
	float m_animationSpeed = 1.0f;//再生速度
};