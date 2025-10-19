#include "BlockStateChangeCommand.h"
#include"../Src/Application/GameLogic/StageModel/StageModel.h"

BlockStateChangeCommand::BlockStateChangeCommand(std::shared_ptr<StageModel> spModel, const std::map<UINT, BlockState>& preStates, const std::map<UINT, BlockState>& newStates) :
	m_wpModel(spModel), m_preStates(preStates), m_newStates(newStates)
{
}

BlockStateChangeCommand::BlockStateChangeCommand(std::shared_ptr<StageModel> spModel, UINT objId, const BlockState& preState, const BlockState& newState):
	m_wpModel(spModel),m_objId(objId)
{
	m_preStates[objId] = preState;
	m_newStates[objId] = newState;
}

void BlockStateChangeCommand::Execute()
{
	//新しい状態で上書き
	if (auto spModel = m_wpModel.lock())
	{
		for (const auto& [id, state] : m_newStates)
		{
			spModel->UpdateBlockState(id, state);
		}
	}
}

void BlockStateChangeCommand::Undo()
{
	//古い状態で上書き
	if (auto spModel = m_wpModel.lock())
	{
		for (const auto& [id, state] : m_preStates)
		{
			spModel->UpdateBlockState(id, state);
		}
	}
}
