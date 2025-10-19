#pragma once
#include"../Command.h"

class StageModel;

class TransformChangeCommand:public ICommand
{
public:
	TransformChangeCommand(std::shared_ptr<StageModel> spModel,
		UINT objId,
		const Math::Vector3& prePos, const Math::Vector3& newPos,
		const Math::Vector3& preRot, const Math::Vector3& newRot,
		const Math::Vector3& preScale, const Math::Vector3& newScale);

	void Execute()override;
	void Undo()override;

private:
	std::weak_ptr<StageModel> m_wpModel;
	UINT m_objId;

	//変更前のデータ
	Math::Vector3 m_prePos, m_newPos;
	Math::Vector3 m_preRot, m_newRot;
	Math::Vector3 m_preScale, m_newScale;
};