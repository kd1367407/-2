#pragma once
#include"../Src/Framework/Component/Component.h"
#include"../../GameLogic/IStageObserver/IStageObserver.h"
#include"../../GameLogic/StageModel/StageModel.h"

class RenderComponent;

class BlockObject:public Component,public IStageObserver
{
public:
	void Init()override;
	void Update()override;

	//Observerとしての機能
	void OnStageStateChanged()override;

	//このブロックがどのモデルのどの座標を担当するかを設定
	//void SetModelInfo(const std::shared_ptr<StageModel>& model, int gridX, int gridY, int gridZ);

private:
	//見た目を更新
//	void UpdateAppearance();

	std::weak_ptr<StageModel> m_wpStageModel;
	//int m_gridX = 0, m_gridY = 0, m_gridZ = 0;
	std::shared_ptr<RenderComponent> m_renderComponent;
};