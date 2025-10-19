#include "TransformChangeCommand.h"
#include"../Src/Application/GameLogic/StageModel/StageModel.h"



TransformChangeCommand::TransformChangeCommand(std::shared_ptr<StageModel> spModel, UINT objId, const Math::Vector3& prePos, const Math::Vector3& newPos, const Math::Vector3& preRot, const Math::Vector3& newRot, const Math::Vector3& preScale, const Math::Vector3& newScale):
	m_wpModel(spModel), m_objId(objId), m_prePos(prePos), m_newPos(newPos), m_preRot(preRot), m_newRot(newRot), m_preScale(preScale), m_newScale(newScale)
{
}

void TransformChangeCommand::Execute()
{
	if (auto spModel = m_wpModel.lock())
	{
		if (BlockState* state = spModel->GetBlockState_Nonconst(m_objId))
		{
			state->pos = m_newPos;
			state->rot = m_newRot;
			state->scale = m_newScale;
		}

		spModel->NotifyObservers();
	}
}

void TransformChangeCommand::Undo()
{
	if (auto spModel = m_wpModel.lock())
	{
		if (BlockState* state = spModel->GetBlockState_Nonconst(m_objId)) {
			state->pos = m_prePos;
			state->rot = m_preRot;
			state->scale = m_preScale;
		}
		spModel->NotifyObservers();
	}
}
