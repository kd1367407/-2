#pragma once
#include"../Command.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"

class GameViewModel;

class DeleteObjectCommand:public ICommand
{
public:
	DeleteObjectCommand(std::shared_ptr<GameViewModel> viewModel, std::vector<UINT>& objectIds, const std::vector<BlockState>& deletedObjStates);
	void Execute()override;
	void Undo()override;

private:
	std::weak_ptr<GameViewModel> m_wpViewModel;
	std::vector<UINT> m_objectIds;
	std::vector<BlockState> m_deletedObjectStates;
};