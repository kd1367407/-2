#include "CreateObjectCommand.h"
#include"../Src/Application/GameViewModel.h"

CreateObjectCommand::CreateObjectCommand(const std::shared_ptr<GameViewModel>& viewModel, const std::vector<UINT>& createIds):
	m_wpViewModel(viewModel),m_createdIDs(createIds)
{
}

void CreateObjectCommand::Execute()
{
	//オブジェクトはViewModelで生成されるので何もしない。
}

void CreateObjectCommand::Undo()
{
	if (auto spViewModel = m_wpViewModel.lock())
	{
		// 生成した全てのオブジェクトをIDで指定して削除依頼
		for (UINT id : m_createdIDs)
		{
			spViewModel->FinalizeObjectDeletion(id);
		}
	}
}
