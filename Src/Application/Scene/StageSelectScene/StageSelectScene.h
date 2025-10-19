#pragma once
#include"../BaseScene/BaseScene.h"

class StageSelectScene :public BaseScene
{
public:
	void Init()override;
	void SceneUpdate()override;
	void Draw()override;
	void Release()override;

private:

	enum class ViewMode
	{
		Main,
		Tutorial
	};

	void DrawStageButtons(const std::string& filePath);

	//プレビュー画像生成
	void GenerateStagePreview(const std::string& stagePath);

	ViewMode m_currentView = ViewMode::Main;

	std::map<std::string, std::shared_ptr<KdTexture>> m_stagePreviews;

	float m_buttonAlpha = 0.0f;
};