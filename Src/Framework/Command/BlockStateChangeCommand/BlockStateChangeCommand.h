#pragma once
#include"../Command.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"

class StageModel;

class BlockStateChangeCommand:public ICommand
{
public:
	//複数用
	BlockStateChangeCommand(
		std::shared_ptr<StageModel> spModel,
		const std::map<UINT, BlockState>& preStates,
		const std::map<UINT, BlockState>& newStates
	);

	//単体用
	BlockStateChangeCommand(
		std::shared_ptr<StageModel> spModel,
		UINT objId,
		const BlockState& preState,
		const BlockState& newState);
	void Execute()override;
	void Undo()override;

private:
	std::weak_ptr<StageModel> m_wpModel;
	UINT m_objId;

	std::map<UINT, BlockState> m_preStates;
	std::map<UINT, BlockState> m_newStates;
};