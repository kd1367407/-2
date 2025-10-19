#include "GroupSwapCommand.h"
#include"../Src/Application/GameLogic/StageModel/StageModel.h"

GroupSwapCommand::GroupSwapCommand(std::shared_ptr<StageModel> model, const std::vector<UINT>& groupAIds, const std::vector<UINT>& groupBIds, const Math::Vector3& deltaAtoB):
	m_wpModel(model),m_groupAIds(groupAIds),m_groupBIds(groupBIds),m_delta(deltaAtoB)
{
}

void GroupSwapCommand::Execute()
{
	MoveObject(m_groupAIds, m_delta);
	MoveObject(m_groupBIds, -m_delta);
}

void GroupSwapCommand::Undo()
{
	MoveObject(m_groupAIds, -m_delta);
	MoveObject(m_groupBIds, m_delta);
}

void GroupSwapCommand::MoveObject(const std::vector<UINT>& ids, const Math::Vector3& offset)
{
	if (auto spModel = m_wpModel.lock())
	{
		for (UINT id : ids)
		{
			if (BlockState* state = spModel->GetBlockState_Nonconst(id))
			{
				Math::Vector3 trajectory = state->endPos - state->startPos;

				state->pos += offset;

				if (state->isMovingBlock)
				{
					state->startPos = state->pos;
					state->endPos = state->startPos + trajectory;
				}

				if (state->isSinkingBlock)
				{
					state->sinkingInitialPos = state->pos;
				}

				spModel->NotifyObservers(id);
			}
		}
	}
}
