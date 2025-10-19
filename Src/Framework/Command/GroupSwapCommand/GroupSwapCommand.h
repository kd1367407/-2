#pragma once
#include"../Command.h"

class StageModel;

class GroupSwapCommand:public ICommand
{
public:
	GroupSwapCommand(
		std::shared_ptr<StageModel> model,
		const std::vector<UINT>& groupAIds,
		const std::vector<UINT>& groupBIds,
		const Math::Vector3& deltaAtoB
	);

	void Execute()override;
	void Undo()override;
private:
	void MoveObject(const std::vector<UINT>& ids, const Math::Vector3& offset);

	std::weak_ptr<StageModel> m_wpModel;
	std::vector<UINT> m_groupAIds;
	std::vector<UINT> m_groupBIds;
	Math::Vector3 m_delta;
};