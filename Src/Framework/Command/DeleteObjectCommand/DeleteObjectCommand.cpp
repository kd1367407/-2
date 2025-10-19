#include "DeleteObjectCommand.h"
#include"../Src/Application/GameViewModel.h"
#include"../Src/Application/main.h"

DeleteObjectCommand::DeleteObjectCommand(std::shared_ptr<GameViewModel> viewModel, std::vector<UINT>& objectIds, const std::vector<BlockState>& deletedObjStates):
	m_wpViewModel(viewModel),m_objectIds(objectIds),m_deletedObjectStates(deletedObjStates)
{
}

void DeleteObjectCommand::Execute()
{
	if (auto spViewModel = m_wpViewModel.lock())
	{
		for (UINT id : m_objectIds)
		{
			spViewModel->FinalizeObjectDeletion(id);
		}
	}
}

void DeleteObjectCommand::Undo()
{
	if (auto spViewModel = m_wpViewModel.lock())
	{
		for (const auto& state : m_deletedObjectStates)
		{
			spViewModel->CreateObjectFromState(state);
		}

		spViewModel->PairTransferBlocks();
	}
}
