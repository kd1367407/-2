#pragma once
#include"../../GameData/BlockState/BlockState.h"

class IStageObserver;

//ステージのデータとルールを管理するモデルクラス
class StageModel
{
public:
	void Init();

	//IDで指定された2つのブロックの位置を入れ替える
	bool SwapBlocks(unsigned int id1, unsigned int id2, bool passRules = false);

	//ブロックの状態を追加、削除
	void AddBlockState(const BlockState& state) { m_blockStates[state.entityId] = state; }
	void RemoveBlockState(unsigned int id) { m_blockStates.erase(id); }

	//ViewModelが状態を読み取るためのゲッター
	const std::unordered_map<unsigned int, BlockState>& GetAllBlockState()const { return m_blockStates; }
	const BlockState* GetBlockState(unsigned int id)const;
	BlockState* GetBlockState_Nonconst(UINT id);

	void RegisterObserver(const std::weak_ptr<IStageObserver>& observer) { m_observers.push_back(observer); }

	//ブロックの状態を更新
	void UpdateBlockState(UINT id, const BlockState& newState);

	//登録された全ての観察者に[状態が変化した]と通知
	void NotifyObservers(unsigned int updatedObjectId = UINT_MAX);

private:

	//IDをキーにしたBlockStateのマップで状態管理
	std::unordered_map<unsigned int, BlockState> m_blockStates;

	//観察者リスト
	std::vector<std::weak_ptr<IStageObserver>> m_observers;
};