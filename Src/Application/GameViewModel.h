#pragma once
#include"../Src/Framework/Input/InputReceiver/IInputReceiver.h"
#include"GameLogic/IStageObserver/IStageObserver.h"
#include "SolutionRecorder/SolutionRecorder.h"

//前方宣言
class StageModel;
class CommandInvoker;
class GameObject;
class IdComponent;
class BaseScene;
struct BlockState;
class SolutionRecorder;

class GameViewModel :public IInputReceiver, public IStageObserver, public std::enable_shared_from_this<GameViewModel>
{
public:
	GameViewModel(const std::shared_ptr<StageModel>& model, const std::shared_ptr<CommandInvoker>& invoker, BaseScene* pScene);
	~GameViewModel();

	//ステージの読み込みと生成
	void LoadStage(const std::string stageFilePath);

	//IInputReceiverの実装
	void OnBlockSelected(const std::shared_ptr<GameObject>& selectedObject)override;

	//IStageObserverの実装
	void OnStageStateChanged(unsigned int updateObjectId = UINT_MAX)override;

	//オブジェクトにユニークな名前を与える
	void GenerateUniqueName(const std::shared_ptr<GameObject>& obj, const std::string& name);

	//BaseStage用(entities配列からオブジェクトを生成する)
	void PopulateSceneFromEntities(const nlohmann::json& entitiesArray);

	//削除リクエスト
	void RequestDeleteObject(const std::shared_ptr<GameObject>& objToDelete);

	//実際に削除する
	void FinalizeObjectDeletion(UINT objId);

	//ブロックの状態を取得
	BlockState GetBlockState(UINT objId);
	//const std::unordered_map<unsigned int, BlockState>& GetAllBlockState();

	//現在のステージの状態を指定されたファイルに保存する
	void SaveStage(const std::string& savePath);

	//状態更新(単体)
	void UpdateStateFromGameObject(const std::shared_ptr<GameObject>& obj);

	//状態更新(複数)
	void UpdateStateFromGameObjects(const std::vector<std::shared_ptr<GameObject>> & objs);

	//DeleteObjectCommandのUndoから呼ばれる関数
	std::shared_ptr<GameObject> CreateObjectFromState(const BlockState& state);
	bool IsDirty() const { return m_isDirty; }

	void PairTransferBlocks();
	void PairTransferBlocks(const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2);

	std::vector<std::shared_ptr<GameObject>> CreateObjectForEditor(const std::string& archetypeName);

	//Editorからの座標交換リクエスト
	void RequestSwapBlocks(const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2);

	//グループの入れ替えリクエスト
	void RequestGroupSwap(const std::vector<UINT>& groupA_IDs, const std::vector<UINT>& groupB_IDs, const Math::Vector3& delta);

	//ペースト
	std::vector<std::shared_ptr<GameObject>> CreateObjectsFromClipboard(std::vector<BlockState> clipboardData);

	//解法記録
	void StartSolutionRecording();
	void StopSolutionRecording();
	bool IsSolutionRecording() const;
	void ClearSolutionPath();
	const std::vector<SolutionStep>& GetSolutionSteps()const;
	const std::map<UINT, BlockState>& GetSolutionStartState()const;
	void ForceSetStageState(const std::map<UINT, BlockState>& stateMap);

	std::shared_ptr<GameObject> GetObjectFromID(UINT id);

	const std::shared_ptr<StageModel>& GetModel()const { return m_model; }

	//数字を無視して名前が一致するか判断
	bool DoesObjectExist(const std::string& name) const;

private:
	//ユニークなtransferIDを返す
	int GetNextUniqueTransferID();

	// カメラの追従ターゲットなどを設定
	void SetupCameraTargets();

	std::shared_ptr<StageModel> m_model;
	std::shared_ptr<CommandInvoker> m_invoker;
	BaseScene* m_pScene = nullptr; // 所有権は持たない

	//IDとGameObjectを相互に管理するためのマップ
	std::unordered_map<unsigned int, std::weak_ptr<GameObject>> m_entityMap;

	//選択されたブロックのIDを一時的に保存
	std::vector<unsigned int> m_selectedIds;

	//選択されたブロックの座標を一時的に保存
	std::vector<Math::Vector3> m_selectedPos;

	//オブジェクトの種類ごとに次に使うIDを管理
	std::map<std::string, int> m_objNameCounter;

	//ステージに変更があったか
	bool m_isDirty = false;

	//解法記録クラスのインスタンス
	std::unique_ptr<SolutionRecorder> m_solutionRecorder;

	//解法記録開始時のステージの状態をバックアップ
	std::map<UINT, BlockState> m_solutionStartState;

	static unsigned int s_nextEntityId;
};

