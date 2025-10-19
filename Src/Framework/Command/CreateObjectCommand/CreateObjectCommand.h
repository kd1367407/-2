#pragma once
#include"../Command.h"

class GameViewModel;

class CreateObjectCommand:public ICommand
{
public:
	CreateObjectCommand(const std::shared_ptr<GameViewModel>& viewModel, const std::vector<UINT>& createIds);
	void Execute()override;
	void Undo()override;

private:
	std::weak_ptr<GameViewModel> m_wpViewModel;
	std::vector<UINT> m_createdIDs;//生成したオブジェクトのID
};