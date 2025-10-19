#include "GameViewModel.h"
#include"GameLogic/StageModel/StageModel.h"
#include"../Src/Framework/Command/CommandInvoker/CommandInvoker.h"
#include"../Src/Framework/Command/SwapBlockCommand/SwapBlockCommand.h"
#include"../Src/Framework/GameObject/ArchetypeManager.h"
#include"../Src/Framework/Component/GameObject.h"
#include"../Src/Framework/Component/TransformComponent/TransformComponent.h"
#include"../Src/Framework/Component/BlockDataComponent/BlockDataComponent.h"
#include"../Src/Framework/Component/IdComponent/IdComponent.h"
#include"Scene/SceneManager.h"
#include"Scene/BaseScene/BaseScene.h"
#include"Scene/GameScene/GameScene.h"
#include"../Src/Framework/Component/RigidbodyComponent/RigidbodyComponent.h"
#include"../Src/Framework/Component/MovingBlockComponent/MovingBlockComponent.h"
#include"../Src/Framework/Component/RenderComponent/RenderComponent.h"
#include"../Src/Framework/Component/ColliderComponent/ColliderComponent.h"
#include"../Src/Framework/GameObject/GameObjectFactory.h"
#include"../Src/Framework/Command/DeleteObjectCommand/DeleteObjectCommand.h"
#include"main.h"
#include"../Src/Framework/Command/TransformChangeCommand/TransformChangeCommand.h"
#include"../Src/Framework/Component/TransferBlockComponent/TransferBlockComponent.h"
#include"../Src/Framework/Command/CreateObjectCommand/CreateObjectCommand.h"
#include"JsonHelper/JsonHelper.h"
#include"../Src/Framework/Component/CameraComponent/TPSCameraComponent/TPSCameraComponent.h"
#include"../Src/Framework/Component/InputComponent/PlayerInputComponent.h"
#include"../Src/Framework/Component/PlayerStatsComponent/PlayerStatsComponent.h"
#include"../Src/Framework/JsonConversion/JsonConversion.h"
#include"../Src/Framework/Command/BlockStateChangeCommand/BlockStateChangeCommand.h"
#include"../Src/Framework/Component/JumpBlockComponent/JumpBlockComponent.h"
#include"../Src/Framework/Component/RotatingBlockComponent/RotatingBlockComponent.h"
#include"../Src/Framework/Component/SinkingBlockComponent/SinkingBlockComponent.h"
#include"../Src/Framework/Component/ScalingBlockComponent/ScalingBlockComponent.h"
#include"../Src/Framework/Command/GroupSwapCommand/GroupSwapCommand.h"
#include"SolutionRecorder/SolutionRecorder.h"

unsigned int GameViewModel::s_nextEntityId = 1;
using json = nlohmann::json;

GameViewModel::GameViewModel(const std::shared_ptr<StageModel>& model, const std::shared_ptr<CommandInvoker>& invoker, BaseScene* pScene) :
	m_model(model), m_invoker(invoker), m_pScene(pScene)
{
	m_solutionRecorder = std::make_unique<SolutionRecorder>();
}

GameViewModel::~GameViewModel()
{
}

void GameViewModel::LoadStage(const std::string stageFilePath)
{
	if (!m_model || !m_pScene)return;

	m_model->RegisterObserver(weak_from_this());

	//BaseStage読み込み
	{
		std::ifstream ifs("Asset/Data/Stages/BaseStage.json");
		if (ifs.is_open())
		{
			nlohmann::json baseStageData;
			ifs >> baseStageData;
			if (baseStageData.contains("entities"))
			{
				PopulateSceneFromEntities(baseStageData["entities"]);
			}
		}
	}

	//個別ステージ読み込み
	{
		json stageData;
		std::ifstream ifs(stageFilePath);
		if (!ifs.is_open())
		{
			Application::Instance().AddLog("[Error] Failed to open stage file: %s", stageFilePath.c_str());
			return;
		}
		ifs >> stageData;

		if (stageData.contains("entities"))
		{
			PopulateSceneFromEntities(stageData["entities"]);
		}
	}

	SetupCameraTargets();
	PairTransferBlocks();
}

void GameViewModel::OnBlockSelected(const std::shared_ptr<GameObject>& selectedObject)
{
	if (!selectedObject)return;

	auto idComp = selectedObject->GetComponent<IdComponent>();
	if (!idComp)return;

	UINT selectedId = idComp->GetId();
	auto renderComp = selectedObject->GetComponent<RenderComponent>();
	if (!renderComp)return;

	auto transformComp = selectedObject->GetComponent<TransformComponent>();
	Math::Vector3 selectedPos = transformComp->GetPos();

	//選択リストから探す
	auto it = std::find(m_selectedIds.begin(), m_selectedIds.end(), selectedId);

	//選択済みのオブジェクトをクリックした場合
	if (it != m_selectedIds.end())
	{
		//選択解除
		m_selectedIds.erase(it);
		//ハイライト解除
		renderComp->SetHighlightState(RenderComponent::HighlightState::None);
		return;
	}

	//新たなブロックを選択した場合
	if (m_selectedIds.size() < 2)//2つまで
	{
		KdAudioManager::Instance().Play("Asset/Sound/BockOneSelect3.wav", false, 1.0f);
		m_selectedIds.push_back(selectedId);
		m_selectedPos.push_back(selectedPos);
		renderComp->SetHighlightState(RenderComponent::HighlightState::Selected);
	}

	//2つのオブジェクトが選択されたら
	if (m_selectedIds.size() == 2)
	{
		//コマンド実行
		auto command = std::make_unique<SwapBlockCommand>(m_model, m_selectedIds[0], m_selectedIds[1], false);
		m_invoker->ExecuteCommand(std::move(command));

		//エフェクト
		if (auto scene = dynamic_cast<GameScene*>(SceneManager::Instance().GetCurrentScene()))
		{
			scene->GetParticleSystem()->Emit(
				m_selectedPos[0],      // 発生場所
				10,             // 発生個数
				{ 1, 0.8f, 0, 1 },// 色 (オレンジっぽい光)
				5.0f, 5.0f,     // 速さの範囲
				1.0f, 1.5f,     // 寿命の範囲
				1.0f, 1.5f     // サイズの範囲
			);

			scene->GetParticleSystem()->Emit(
				m_selectedPos[1],      // 発生場所
				10,             // 発生個数
				{ 1, 0.8f, 0, 1 },// 色 (オレンジっぽい光)
				5.0f, 5.0f,     // 速さの範囲
				1.3f, 1.5f,     // 寿命の範囲
				1.0f, 1.5f     // サイズの範囲
			);
		}

		//両方のハイライトを消す
		for (UINT id : m_selectedIds)
		{
			if (auto mapIt = m_entityMap.find(id); mapIt != m_entityMap.end())
			{
				if (auto obj = mapIt->second.lock())
				{
					if (auto rComp = obj->GetComponent<RenderComponent>())
					{
						rComp->SetHighlightState(RenderComponent::HighlightState::None);
					}
				}
			}
		}
		//選択状態リセット
		m_selectedIds.clear();
		m_selectedPos.clear();
		KdAudioManager::Instance().Play("Asset/Sound/BlockTwoSelect.wav", false, 1.5f);
	}
}

//Modelの変更通知を受けてViewを更新
void GameViewModel::OnStageStateChanged(unsigned int updateObjectId)
{
	//全体更新の場合
	if (updateObjectId == UINT_MAX)
	{
		const auto& allState = m_model->GetAllBlockState();
		for (const auto& [id, state] : allState)
		{
			if (auto it = m_entityMap.find(id); it != m_entityMap.end())
			{
				if (auto obj = it->second.lock())
				{
					obj->ApplyState(state);
				}
			}
		}
	}
	//単体更新の場合
	else
	{
		if (const BlockState* state = m_model->GetBlockState(updateObjectId))
		{
			if (auto it = m_entityMap.find(state->entityId); it != m_entityMap.end())
			{
				if (auto obj = it->second.lock())
				{
					obj->ApplyState(*state);
				}
			}
		}
	}
}

void GameViewModel::GenerateUniqueName(const std::shared_ptr<GameObject>& obj, const std::string& name)
{
	if (!obj)return;

	std::string baseName = name;
	size_t underbarPos = baseName.find_last_of('_');

	if (underbarPos != std::string::npos)
	{
		std::string suffix = baseName.substr(underbarPos + 1);//'_'以降の数字を取得
		if (!suffix.empty() && std::all_of(suffix.begin(), suffix.end(), ::isdigit))//'_'以降の文字が数字オンリーか
		{
			baseName = baseName.substr(0, underbarPos);
		}
	}

	//型名(baseName)をキーにしてカウンターから次のIDを取得してカウンターを増やす
	int& id = m_objNameCounter[baseName];
	obj->SetName(baseName + "_" + std::to_string(id++));
}

void GameViewModel::PopulateSceneFromEntities(const nlohmann::json& entitiesArray)
{
	if (!entitiesArray.is_array())return;

	GameObjectFactory factory;

	for (const auto& entityData : entitiesArray)
	{
		std::string archetypeName = entityData.value("archetype", "");
		if (archetypeName.empty())continue;

		auto newObject = factory.CreateGameObject(archetypeName, entityData);

		if (newObject)
		{
			if (auto transform = newObject->GetComponent<TransformComponent>())
			{
				transform->SetViewModel(shared_from_this());
			}

			if (auto transfer = newObject->GetComponent<TransferBlockComponent>())
			{
				transfer->SetViewModel(shared_from_this());
			}

			if (auto movingBlock = newObject->GetComponent<MovingBlockComponent>())
			{
				movingBlock->SetViewModel(shared_from_this());
			}

			if (auto jumpBlock = newObject->GetComponent<JumpBlockComponent>())
			{
				jumpBlock->SetViewModel(shared_from_this());
			}

			if (auto rotatingBlock = newObject->GetComponent<RotatingBlockComponent>())
			{
				rotatingBlock->SetViewModel(shared_from_this());
			}

			if (auto sinkingBlock = newObject->GetComponent<SinkingBlockComponent>())
			{
				sinkingBlock->SetViewModel(shared_from_this());
			}

			if (auto scalingBlock = newObject->GetComponent<ScalingBlockComponent>())
			{
				scalingBlock->SetViewModel(shared_from_this());
			}

			if (auto playerInput = newObject->GetComponent<PlayerInputComponent>())
			{
				playerInput->SetInvoker(m_invoker);
			}

			if (newObject->GetComponent<BlockDataComponent>())
			{
				GenerateUniqueName(newObject, newObject->GetName());
			}

			m_pScene->AddObject(newObject);

			if (newObject->GetComponent<BlockDataComponent>() || archetypeName == "Player" /*|| archetypeName == "PreviewCamera"*/)
			{
				BlockState state = newObject->CreateState();

				UINT newId = s_nextEntityId++;
				state.entityId = newId;
				if (auto idComp = newObject->GetComponent<IdComponent>())
				{
					idComp->SetID(newId);
				}

				m_model->AddBlockState(state);
				m_entityMap[newId] = newObject;
			}
		}
	}
}

void GameViewModel::RequestDeleteObject(const std::shared_ptr<GameObject>& objToDelete)
{
	if (!objToDelete)return;

	std::vector<std::shared_ptr<GameObject>> objectsToDelete;
	objectsToDelete.push_back(objToDelete);

	//削除不可能オブジェクトかチェック
	std::string name = objToDelete->GetName();
	if (name == "Player" || name._Starts_with("Goal"))
	{
		Application::Instance().AddLog("This object cannot be deleted.");
		return;
	}

	//Transferならパートナーも削除対象に
	if (auto transferComp = objToDelete->GetComponent<TransferBlockComponent>())
	{
		if (auto partner = transferComp->GetPartner())
		{
			objectsToDelete.push_back(partner);
		}
	}

	//アンドゥ用のデータを取得
	std::vector<UINT> idsToDelete;
	std::vector<BlockState> statesToUndo;
	for (const auto& obj : objectsToDelete)
	{
		if (auto idComp = obj->GetComponent<IdComponent>())
		{
			UINT id = idComp->GetId();
			idsToDelete.push_back(id);
			statesToUndo.push_back(GetBlockState(id));
		}
	}

	if (idsToDelete.empty())return;

	//コマンド生成
	auto command = std::make_unique<DeleteObjectCommand>(shared_from_this(), idsToDelete, statesToUndo);
	m_invoker->ExecuteCommand(std::move(command));

	m_isDirty = true;
}

void GameViewModel::FinalizeObjectDeletion(UINT objId)
{
	//Modelから状態データを削除
	m_model->RemoveBlockState(objId);

	//View(gameobject)を削除
	if (auto it = m_entityMap.find(objId); it != m_entityMap.end())
	{
		if (auto obj = it->second.lock())
		{
			obj->Expired();
		}
		m_entityMap.erase(it);
	}
}

BlockState GameViewModel::GetBlockState(UINT objId)
{
	if (const BlockState* state = m_model->GetBlockState(objId))
	{
		return *state;
	}
	return {};
}

//const std::unordered_map<unsigned int, BlockState>& GameViewModel::GetAllBlockState()
//{
//	return m_model->GetAllBlockState();
//}

void GameViewModel::SaveStage(const std::string& savePath)
{
	if (!m_pScene)return;

	nlohmann::json stageJson;
	stageJson["entities"] = nlohmann::json::array();

	for (const auto& obj : m_pScene->GetObjList())
	{
		const Archetype* archetype = ArchetypeManager::Instance().GetArchetype(obj->GetArchetypeName());
		if (archetype && archetype->IsSavable())
		{
			stageJson["entities"].push_back(obj->ToJson());
		}
	}

	std::ofstream ofs(savePath);
	if (ofs) {
		ofs << std::setw(4) << stageJson << std::endl;
	}
	Application::Instance().AddLog("Save Scene to %s", savePath.c_str());
	m_isDirty = false;
}

void GameViewModel::UpdateStateFromGameObject(const std::shared_ptr<GameObject>& obj)
{
	if (!obj) return;
	if (auto idComp = obj->GetComponent<IdComponent>())
	{
		UINT id = idComp->GetId();

		//変更前の状態取得
		const BlockState* preStatePtr = m_model->GetBlockState(id);
		if (!preStatePtr)return;
		BlockState preState = *preStatePtr;//ポインタの中身を取り出す

		//変更後の状態を生成
		BlockState newState = obj->CreateState();
		newState.entityId = id; // IDは維持

		auto command = std::make_unique<BlockStateChangeCommand>(m_model, id, preState, newState);
		m_invoker->ExecuteCommand(std::move(command));

		m_isDirty = true;
	}
}

void GameViewModel::UpdateStateFromGameObjects(const std::vector<std::shared_ptr<GameObject>>& objs)
{
	if (objs.empty())return;

	std::map<UINT, BlockState> preStates;
	std::map<UINT, BlockState> newStates;

	//全オブジェクトの変更前後のStateを取得
	for (const auto& obj : objs)
	{
		if (!obj)continue;
		if (auto idComp = obj->GetComponent<IdComponent>())
		{
			UINT id = idComp->GetId();

			//変更前のState取得
			if (const BlockState* preStatePtr = m_model->GetBlockState(id))
			{
				preStates[id] = *preStatePtr;

				//変更後のState生成
				newStates[id] = obj->CreateState();
			}
		}
	}

	if (preStates.empty() || newStates.empty())return;

	//コマンド発行
	auto command = std::make_unique<BlockStateChangeCommand>(m_model, preStates, newStates);
	m_invoker->ExecuteCommand(std::move(command));

	m_isDirty = true;
}

std::shared_ptr<GameObject> GameViewModel::CreateObjectFromState(const BlockState& state)
{
	m_isDirty = true;

	nlohmann::json entityData = state;

	GameObjectFactory factory;
	auto newObject = factory.CreateGameObject(state.archetypeName, entityData);

	if (newObject)
	{
		if (auto transform = newObject->GetComponent<TransformComponent>())
		{
			transform->SetViewModel(shared_from_this());
		}

		if (auto transfer = newObject->GetComponent<TransferBlockComponent>())
		{
			transfer->SetViewModel(shared_from_this());
		}

		if (auto movingBlock = newObject->GetComponent<MovingBlockComponent>())
		{
			movingBlock->SetViewModel(shared_from_this());
		}

		if (auto jumpBlock = newObject->GetComponent<JumpBlockComponent>())
		{
			jumpBlock->SetViewModel(shared_from_this());
		}

		if (auto rotatingBlock = newObject->GetComponent<RotatingBlockComponent>())
		{
			rotatingBlock->SetViewModel(shared_from_this());
		}

		if (auto sinkingBlock = newObject->GetComponent<SinkingBlockComponent>())
		{
			sinkingBlock->SetViewModel(shared_from_this());
		}

		if (auto scalingBlock = newObject->GetComponent<ScalingBlockComponent>())
		{
			scalingBlock->SetViewModel(shared_from_this());
		}

		m_pScene->AddObject(newObject);

		//Modelに再登録
		m_model->AddBlockState(state);

		if (auto idComp = newObject->GetComponent<IdComponent>())
		{
			idComp->SetID(state.entityId);
		}
		m_entityMap[state.entityId] = newObject;

		newObject->Init();

		//現在のs_nextEntityIdが復元したIDより小さくならないように更新
		if (state.entityId >= s_nextEntityId)
		{
			s_nextEntityId = state.entityId + 1;
		}
	}

	return newObject;
}

void GameViewModel::PairTransferBlocks()
{
	std::unordered_map<int, std::vector<std::shared_ptr<GameObject>>> transferMap;

	if (!m_pScene)return;

	//IDごとにGameObjectを分類
	for (const auto& obj : m_pScene->GetObjList())
	{
		if (auto transferComp = obj->GetComponent<TransferBlockComponent>())
		{
			int id = transferComp->GetTransferID();
			if (id > 0)
			{
				transferMap[id].push_back(obj);
			}
		}
	}

	//ペア設定
	for (auto const& [id, objects] : transferMap)
	{
		if (objects.size() == 2)
		{
			auto compA = objects[0]->GetComponent<TransferBlockComponent>();
			auto compB = objects[1]->GetComponent<TransferBlockComponent>();

			//ペアリング
			if (compA && compB)
			{
				compA->SetPartner(objects[1]);
				compB->SetPartner(objects[0]);
			}
		}
	}
}

void GameViewModel::PairTransferBlocks(const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2)
{
	if (!obj1 || !obj2)return;

	auto comp1 = obj1->GetComponent<TransferBlockComponent>();
	auto comp2 = obj2->GetComponent<TransferBlockComponent>();

	if (comp1 && comp2)
	{
		comp1->SetPartner(obj2);
		comp2->SetPartner(obj1);
	}
}

int GameViewModel::GetNextUniqueTransferID()
{
	int maxID = 0;
	if (!m_pScene)return 1;

	for (const auto& obj : m_pScene->GetObjList())
	{
		if (auto transferComp = obj->GetComponent<TransferBlockComponent>())
		{
			if (transferComp->GetTransferID() > maxID)
			{
				maxID = transferComp->GetTransferID();
			}
		}
	}
	return maxID + 1;
}

std::vector<std::shared_ptr<GameObject>> GameViewModel::CreateObjectForEditor(const std::string& archetypeName)
{
	if (archetypeName == "PreviewCamera" && DoesObjectExist("PreviewCamera"))
	{
		Application::Instance().AddLog("PreviewCamera already exists. Creation cancelled.");
		return {};//空のベクターを返す
	}

	//ファクトリにオブジェクト生成命令
	GameObjectFactory factory;
	std::vector<std::shared_ptr<GameObject>> createdObjects;

	if (archetypeName == "TransferBlock")
	{
		int newTransferId = GetNextUniqueTransferID();

		//1つ目
		nlohmann::json data1;
		data1["components"]["TransferBlockComponent"]["transferID"] = newTransferId;
		auto obj1 = factory.CreateGameObject(archetypeName, data1);

		//2つ目
		nlohmann::json data2;
		data2["components"]["TransformComponent"]["position"] = { 4.0f,2.0f,0.0f };
		data2["components"]["TransferBlockComponent"]["transferID"] = newTransferId;
		auto obj2 = factory.CreateGameObject(archetypeName, data2);

		if (obj1 && obj2)
		{
			createdObjects.push_back(obj1);
			createdObjects.push_back(obj2);

			PairTransferBlocks(obj1, obj2);
		}
	}
	else
	{
		auto newObj = factory.CreateGameObject(archetypeName);
		if (newObj)
		{
			createdObjects.push_back(newObj);
		}
	}

	std::vector<UINT> allCreatedIds;
	for (const auto& newObject : createdObjects)
	{
		GenerateUniqueName(newObject, newObject->GetName());

		if (auto transform = newObject->GetComponent<TransformComponent>())
		{
			transform->SetViewModel(shared_from_this());
		}

		if (auto transfer = newObject->GetComponent<TransferBlockComponent>())
		{
			transfer->SetViewModel(shared_from_this());
		}

		if (auto movingBlock = newObject->GetComponent<MovingBlockComponent>())
		{
			movingBlock->SetViewModel(shared_from_this());
		}

		if (auto jumpBlock = newObject->GetComponent<JumpBlockComponent>())
		{
			jumpBlock->SetViewModel(shared_from_this());
		}

		if (auto rotatingBlock = newObject->GetComponent<RotatingBlockComponent>())
		{
			rotatingBlock->SetViewModel(shared_from_this());
		}

		if (auto sinkingBlock = newObject->GetComponent<SinkingBlockComponent>())
		{
			sinkingBlock->SetViewModel(shared_from_this());
		}

		if (auto scalingBlock = newObject->GetComponent<ScalingBlockComponent>())
		{
			scalingBlock->SetViewModel(shared_from_this());
		}
		
		m_pScene->AddObject(newObject);

		UINT newId = 0;

		//アンドゥのためにIDを取得
		if (auto idComp = newObject->GetComponent<IdComponent>())
		{
			if (idComp->GetId() == 0)
			{
				idComp->SetID(s_nextEntityId++);
			}
			newId = idComp->GetId();
			allCreatedIds.push_back(newId);
		}

		//BlockStateを生成してModelに登録
		BlockState state = newObject->CreateState();
		state.entityId = newId;
		m_model->AddBlockState(state);
		m_entityMap[newId] = newObject;
	}

	//コマンド生成(生成したオブジェクトのIDを伝える)
	auto command = std::make_unique<CreateObjectCommand>(shared_from_this(), allCreatedIds);
	m_invoker->ExecuteCommand(std::move(command));

	m_isDirty = true;

	return createdObjects;
}

void GameViewModel::RequestSwapBlocks(const std::shared_ptr<GameObject>& obj1, const std::shared_ptr<GameObject>& obj2)
{
	if (!obj1 || !obj2 || !m_invoker) return;

	auto idComp1 = obj1->GetComponent<IdComponent>();
	auto idComp2 = obj2->GetComponent<IdComponent>();

	if (!idComp1 || !idComp2)return;

	UINT id1 = idComp1->GetId();
	UINT id2 = idComp2->GetId();

	auto command = std::make_unique<SwapBlockCommand>(m_model, id1, id2, true);
	m_invoker->ExecuteCommand(std::move(command));

	m_isDirty = true;
	Application::Instance().AddLog("Swapped positions of %s and %s", obj1->GetName().c_str(), obj2->GetName().c_str());
}

void GameViewModel::RequestGroupSwap(const std::vector<UINT>& groupA_IDs, const std::vector<UINT>& groupB_IDs, const Math::Vector3& delta)
{
	if (!m_invoker || !m_model) return;

	bool isSingleObjectSwap = groupA_IDs.size() == 1 && groupB_IDs.size() == 1;

	if (isSingleObjectSwap && m_solutionRecorder->IsRecording())
	{
		m_solutionRecorder->AddStep(groupA_IDs[0], groupB_IDs[0]);
		Application::Instance().AddLog("Solution step recorded: %u <-> %u", groupA_IDs[0], groupB_IDs[0]);
	}

	auto command = std::make_unique<GroupSwapCommand>(m_model, groupA_IDs, groupB_IDs, delta);
	m_invoker->ExecuteCommand(std::move(command));
	m_isDirty = true;
}

std::vector<std::shared_ptr<GameObject>> GameViewModel::CreateObjectsFromClipboard(std::vector<BlockState> clipboardData)
{
	std::vector<std::shared_ptr<GameObject>> createdObjects;
	if (clipboardData.empty())return createdObjects;

	for (auto state : clipboardData)
	{
		if (state.archetypeName == "PreviewCamera")
		{
			if (DoesObjectExist("PreviewCamera"))
			{
				Application::Instance().AddLog("PreviewCamera already exists. Paste operation for this object was skipped.");
				continue;
			}
		}

		if (state.archetypeName == "Player")
		{
			if (DoesObjectExist("Player"))
			{
				Application::Instance().AddLog("Player already exists. Paste operation for this object was skipped.");
				continue;
			}
		}

		if (state.archetypeName == "GoalBlock")
		{
			if (DoesObjectExist("GoalBlock"))
			{
				Application::Instance().AddLog("GoalBlock already exists. Paste operation for this object was skipped.");
				continue;
			}
		}

		state.entityId = s_nextEntityId;

		auto newObject = CreateObjectFromState(state);

		if (newObject)
		{
			GenerateUniqueName(newObject, newObject->GetArchetypeName());
			createdObjects.push_back(newObject);
		}
	}

	//アンドゥ対応
	std::vector<UINT> createdIDs;
	for (const auto& obj : createdObjects)
	{
		if (auto idComp = obj->GetComponent<IdComponent>())
		{
			createdIDs.push_back(idComp->GetId());
		}
	}
	if (!createdIDs.empty())
	{
		auto command = std::make_unique<CreateObjectCommand>(shared_from_this(), createdIDs);
		m_invoker->ExecuteCommand(std::move(command));
		m_isDirty = true;
	}
	return createdObjects;
}

void GameViewModel::StartSolutionRecording()
{
	if (m_solutionRecorder)
	{
		m_solutionRecorder->StartRecrding();

		//バックアップ取得
		m_solutionStartState.clear();
		const auto& allStates = m_model->GetAllBlockState();
		for (const auto& [id, state] : allStates)
		{
			m_solutionStartState[id] = state;
		}
	}
	Application::Instance().AddLog("Solution recording started. Initial state backed up.");
}

void GameViewModel::StopSolutionRecording()
{
	if (m_solutionRecorder)
	{
		m_solutionRecorder->StopRecording();
	}
	Application::Instance().AddLog("Solution recording stopped.");
}

bool GameViewModel::IsSolutionRecording() const
{
	if (m_solutionRecorder)
	{
		return m_solutionRecorder->IsRecording();
	}
	return false;
}

void GameViewModel::ClearSolutionPath()
{
	if (m_solutionRecorder)
	{
		m_solutionRecorder->Clear();
	}
	Application::Instance().AddLog("Solution path clear.");
}

const std::vector<SolutionStep>& GameViewModel::GetSolutionSteps() const
{
	if (m_solutionRecorder)
	{
		return m_solutionRecorder->GetSolutionSteps();
	}

	static const std::vector<SolutionStep> empty_vector;
	return empty_vector;
}

const std::map<UINT, BlockState>& GameViewModel::GetSolutionStartState() const
{
	return m_solutionStartState;
}

void GameViewModel::ForceSetStageState(const std::map<UINT, BlockState>& stateMap)
{
	if (!m_model)return;

	for (const auto& [id, state] : stateMap)
	{
		m_model->UpdateBlockState(id, state);
	}
}

std::shared_ptr<GameObject> GameViewModel::GetObjectFromID(UINT id)
{
	auto it = m_entityMap.find(id);
	if (it != m_entityMap.end())
	{
		return it->second.lock();
	}
	return nullptr;
}

bool GameViewModel::DoesObjectExist(const std::string& name) const
{
	if (!m_pScene) return false;

	for (const auto& obj : m_pScene->GetObjList())
	{
		// オブジェクト名が前方一致するかチェック
		if (obj && obj->GetName()._Starts_with(name))
		{
			return true;
		}
	}
	return false;
}

void GameViewModel::SetupCameraTargets()
{
	if (!m_pScene) return;

	std::shared_ptr<GameObject> player;
	std::shared_ptr<TPSCameraComponent> tpsCamera;

	// まずPlayerとTPS_Cameraを見つける
	for (const auto& obj : m_pScene->GetObjList())
	{
		if (obj->GetName() == "Player")
		{
			player = obj;
		}
		if (auto camComp = obj->GetComponent<TPSCameraComponent>())
		{
			tpsCamera = camComp;
		}
	}

	// 両方見つかったらターゲットを設定
	if (player && tpsCamera)
	{
		if (auto playerTransform = player->GetComponent<TransformComponent>())
		{
			tpsCamera->SetTarget(playerTransform);
			if (auto playerInput = player->GetComponent<PlayerInputComponent>())
			{
				playerInput->SetCamera(tpsCamera);
			}
		}
	}
}

