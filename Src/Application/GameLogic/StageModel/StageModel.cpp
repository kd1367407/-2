#include "StageModel.h"
#include"../IStageObserver/IStageObserver.h"
#include"../Src/Framework/JsonConversion/JsonConversion.h"

void StageModel::Init()
{
	m_blockStates.clear();
}

bool StageModel::SwapBlocks(unsigned int id1, unsigned int id2, bool passRules)
{
	auto it1 = m_blockStates.find(id1);
	auto it2 = m_blockStates.find(id2);

	if (it1 == m_blockStates.end() || it2 == m_blockStates.end())return false;

	//入れ替え可能フラグで判定
	if (!passRules && (!it1->second.isSwappable || !it2->second.isSwappable))
	{
		return false;
	}

	//入れ替える"前"の移動ベクトルをそれぞれ計算して保存
	Math::Vector3 trajectory1 = it1->second.endPos - it1->second.startPos;
	Math::Vector3 trajectory2 = it2->second.endPos - it2->second.startPos;

	//位置のみ入れ替える
	std::swap(it1->second.pos, it2->second.pos);
	/*std::swap(it1->second.isMovingBlock, it2->second.isMovingBlock);
	std::swap(it1->second.renderModelPath, it2->second.renderModelPath);
	std::swap(it1->second.collisionModelPath, it2->second.collisionModelPath);
	std::swap(it1->second.shapeType, it2->second.shapeType);
	std::swap(it1->second.radius, it2->second.radius);*/

	//もし動くブロックなら、新しい位置に合わせて軌道を再計算する
	if (it1->second.isMovingBlock)
	{
		//新しい位置を始点として移動ベクトルを使って終点を再設定
		it1->second.startPos = it1->second.pos;
		it1->second.endPos = it1->second.pos + trajectory1;
	}

	if (it2->second.isMovingBlock)
	{
		it2->second.startPos = it2->second.pos;
		it2->second.endPos = it2->second.pos + trajectory2;
	}

	//Modelが変化したと通知
	NotifyObservers(id1);
	NotifyObservers(id2);

	return true;
}

const BlockState* StageModel::GetBlockState(unsigned int id) const
{
	auto it = m_blockStates.find(id);
	if (it != m_blockStates.end())
	{
		return &it->second;
	}
	return nullptr;
}

BlockState* StageModel::GetBlockState_Nonconst(UINT id)
{
	auto it = m_blockStates.find(id);
	if (it != m_blockStates.end())
	{
		return &it->second;
	}
	return nullptr;
}



void StageModel::UpdateBlockState(UINT id, const BlockState& newState)
{
	auto it = m_blockStates.find(id);
	if (it != m_blockStates.end())
	{
		it->second = newState;
		NotifyObservers(id);
	}
}

void StageModel::NotifyObservers(unsigned int updatedObjectId)
{
	//登録されている全ての観察者に通知する
	for (auto it = m_observers.begin(); it != m_observers.end();)
	{
		if (auto spObs = it->lock())
		{
			spObs->OnStageStateChanged(updatedObjectId);
			++it;
		}
		else
		{
			//寿命切れのObserverをリストから削除
			it = m_observers.erase(it);
		}
	}
}
