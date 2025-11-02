#include "ArchetypeManager.h"
#include"../Component/GameObject.h"
#include"../Component/TransformComponent/TransformComponent.h"
#include"../Component/RenderComponent/RenderComponent.h"
#include"../Component/ImGuiComponent/ImGuiComponent.h"
#include"../Component/BlockDataComponent/BlockDataComponent.h"
#include"../Component/IdComponent/IdComponent.h"
#include"../Component/ColliderComponent/ColliderComponent.h"
#include"../Component/RigidbodyComponent/RigidbodyComponent.h"
#include"../Component/MovingBlockComponent/MovingBlockComponent.h"
#include"../Component/TransferBlockComponent/TransferBlockComponent.h"
#include"../Component/JumpBlockComponent/JumpBlockComponent.h"
#include"../Component/SlipperyComponent/SlipperyComponent.h"
#include"../Component/RotatingBlockComponent/RotatingBlockComponent.h"
#include"../Component/SinkingBlockComponent/SinkingBlockComponent.h"
#include"../Component/ScalingBlockComponent/ScalingBlockComponent.h"
#include"../Component/CheckpointComponent/CheckpointComponent.h"
#include"../Src/Framework/Component/TutorialTriggerComponent/TutorialTriggerComponent.h"
#include"../Src/Framework/Component/CameraComponent/TPSCameraComponent/TPSCameraComponent.h"
#include"../Src/Framework/Component/TagComponent/TagComponent.h"
#include"../Src/Framework/Component/MagicCircleComponent/MagicCircleComponent.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"
#include "../../Application/main.h"

//Component Generation
#include "../Component/CameraComponent/CameraComponent.h"
#include "../Component/CameraComponent/EditorCameraComponent/EditorCameraComponent.h"
#include "../Component/GravityComponent/GravityComponent.h"
#include "../Component/InputComponent/PlayerInputComponent.h"
#include "../Component/MousePointerComponent/MousePointerComponent.h"
#include "../Component/PlayerStatsComponent/PlayerStatsComponent.h"
#include "../Component/SkydomeComponent/SkydomeComponent.h"
#include "../Component/TimerComponent/TimerComponent.h"
using json = nlohmann::json;

ArchetypeManager& ArchetypeManager::Instance()
{
	static ArchetypeManager instance;
	return instance;
}

void ArchetypeManager::Init()
{
	RegisterComponentTypes();

	std::ifstream ifs("Asset/Data/Settings/Archetypes.json");
	if (!ifs.is_open())
	{
		//致命的なエラーなので、ログを出して終了する
		MessageBoxA(nullptr, "Archetypes.jsonが開けませんでした。", "エラー", MB_OK);
		assert(0 && "Archetypes.json not found!");
		return;
	}

	json archetypeJson;
	ifs >> archetypeJson;

	for (auto& [archetypeName, archetypeData] : archetypeJson.items())
	{
		Archetype archetype;

		archetype.m_defaultName = JsonHelper::GetString(archetypeData, "default_name", archetypeName);
		archetype.m_isSpawnableInEditor = JsonHelper::GetBool(archetypeData, "spawnable_in_editor", true);
		archetype.m_isSwappable = JsonHelper::GetBool(archetypeData, "swappable", false);
		archetype.m_isSavable = JsonHelper::GetBool(archetypeData, "savable", true);

		if (archetypeData.contains("components"))
		{
			const auto& componentsData = archetypeData.at("components");

			//汎用的な知識として、JSONデータを丸ごと保存
			archetype.m_componentData = componentsData;

			for (auto [compName, compData] : componentsData.items())
			{
				if (m_componentFactory.count(compName))
				{
					archetype.m_componentCreators.push_back(m_componentFactory[compName]);
				}
				else
				{
					//JSONに記述されているが存在しないコンポーネントの場合、エラーログを出力
					std::string errorMsg = "ArchetypeManager: Component type '" + compName + "' not found in factory for archetype '" + archetypeName + "'. Check Archetypes.json.";
					Application::Instance().AddLog(errorMsg.c_str());
				}

				//重要な知識として、特定のデフォルト値を解析して別途保存
				if (compName == "RenderComponent")
				{
					archetype.m_defaultRenderModelPath = JsonHelper::GetString(compData, "model", "");
				}
				if (compName == "ColliderComponent")
				{
					archetype.m_defaultColModelPath = JsonHelper::GetString(compData, "model", "");
					std::string shapeStr = JsonHelper::GetString(compData, "shape", "mesh");
					if (shapeStr == "sphere") archetype.m_defaultShapeType = Shape::Type::Sphere;
					else if (shapeStr == "box") archetype.m_defaultShapeType = Shape::Type::Box;
					else archetype.m_defaultShapeType = Shape::Type::Mesh;
				}
				if (compName == "RigidbodyComponent")
				{
					std::string typeStr = JsonHelper::GetString(compData, "type", "static");
					if (typeStr == "dynamic") archetype.m_defaultRigidbodyType = RigidbodyType::Dynamic;
					else if (typeStr == "kinematic") archetype.m_defaultRigidbodyType = RigidbodyType::Kinematic;
					else archetype.m_defaultRigidbodyType = RigidbodyType::Static;
				}
			}
		}

		m_archetypes[archetypeName] = archetype;
	}
	Application::Instance().AddLog("Loaded %zu archetypes.", m_archetypes.size());
}

const Archetype* ArchetypeManager::GetArchetype(const std::string& name) const
{
	auto it = m_archetypes.find(name);
	if (it != m_archetypes.end())
	{
		return &it->second;
	}
	return nullptr;
}

std::vector<std::string> ArchetypeManager::GetAllArchetypeNames(bool editorOnly) const
{
	std::vector<std::string> names;
	names.reserve(m_archetypes.size());
	for (const auto& [name, archetype] : m_archetypes)
	{
		//Jsonで設定したフラグでフィルタリング
		if (archetype.GetSpawnInEditor())
		{
			names.push_back(name);
		}
	}
	return names;
}

void ArchetypeManager::RegisterComponentTypes()
{
	m_componentFactory["TransformComponent"] = []() { return std::make_shared<TransformComponent>(); };
	m_componentFactory["RenderComponent"] = []() { return std::make_shared<RenderComponent>(); };
	m_componentFactory["ImGuiComponent"] = []() { return std::make_shared<ImGuiComponent>(); };
	m_componentFactory["BlockDataComponent"] = []() { return std::make_shared<BlockDataComponent>(); };
	m_componentFactory["IdComponent"] = []() { return std::make_shared<IdComponent>(); };
	m_componentFactory["ColliderComponent"] = []() { return std::make_shared<ColliderComponent>(); };
	m_componentFactory["RigidbodyComponent"] = []() { return std::make_shared<RigidbodyComponent>(); };
	m_componentFactory["MovingBlockComponent"] = []() { return std::make_shared<MovingBlockComponent>(); };
	m_componentFactory["TransferBlockComponent"] = []() { return std::make_shared<TransferBlockComponent>(); };
	m_componentFactory["JumpBlockComponent"] = []() { return std::make_shared<JumpBlockComponent>(); };
	m_componentFactory["PlayerInputComponent"] = []() { return std::make_shared<PlayerInputComponent>(); };
	m_componentFactory["PlayerStatsComponent"] = []() { return std::make_shared<PlayerStatsComponent>(); };
	m_componentFactory["GravityComponent"] = []() { return std::make_shared<GravityComponent>(); };
	m_componentFactory["MousePointerComponent"] = []() { return std::make_shared<MousePointerComponent>(); };
	m_componentFactory["CameraComponent"] = []() { return std::make_shared<CameraComponent>(); };
	m_componentFactory["EditorCameraComponent"] = []() { return std::make_shared<EditorCameraComponent>(); };
	m_componentFactory["TimerComponent"] = []() { return std::make_shared<TimerComponent>(); };
	m_componentFactory["SkydomeComponent"] = []() { return std::make_shared<SkydomeComponent>(); };
	m_componentFactory["SlipperyComponent"] = []() { return std::make_shared<SlipperyComponent>(); };
	m_componentFactory["RotatingBlockComponent"] = []() { return std::make_shared<RotatingBlockComponent>(); };
	m_componentFactory["SinkingBlockComponent"] = []() { return std::make_shared<SinkingBlockComponent>(); };
	m_componentFactory["ScalingBlockComponent"] = []() { return std::make_shared<ScalingBlockComponent>(); };
	m_componentFactory["CheckpointComponent"] = []() { return std::make_shared<CheckpointComponent>(); };
	m_componentFactory["TutorialTriggerComponent"] = []() { return std::make_shared<TutorialTriggerComponent>(); };
	m_componentFactory["TPSCameraComponent"] = []() { return std::make_shared<TPSCameraComponent>(); };
	m_componentFactory["TagComponent"] = []() { return std::make_shared<TagComponent>(); };
	m_componentFactory["MagicCircleComponent"] = []() { return std::make_shared<MagicCircleComponent>(); };
}

void ArchetypeManager::ApplyComponentSettings(GameObject& obj, const nlohmann::json& componentData)
{
	for (auto& [compName, compData] : componentData.items())
	{
		// RenderComponentの設定
		if (compName == "RenderComponent")
		{
			if (auto comp = obj.GetComponent<RenderComponent>())
			{
				std::string modelPath = JsonHelper::GetString(compData, "default_model");
				if (!modelPath.empty())
				{
					comp->SetModel(KdAssets::Instance().m_modeldatas.GetData(modelPath));
				}
			}
		}
		// BlockDataComponentの設定
		else if (compName == "BlockDataComponent")
		{
			if (auto comp = obj.GetComponent<BlockDataComponent>())
			{
				std::string typeStr = JsonHelper::GetString(compData, "type");
				// 文字列からBlockTypeに変換
				if (typeStr == "Movable") comp->SetType(BlockType::Movable);
				else if (typeStr == "Wall") comp->SetType(BlockType::Wall);
				else if (typeStr == "Goal") comp->SetType(BlockType::Goal);
				else if (typeStr == "Moving") comp->SetType(BlockType::Moving);
				else if (typeStr == "Transfer") comp->SetType(BlockType::Transfer);
				else if (typeStr == "Jump") comp->SetType(BlockType::Jump);
				else if (typeStr == "Slippery") comp->SetType(BlockType::Slippery);
				else if (typeStr == "Rotating") comp->SetType(BlockType::Rotating);
				else comp->SetType(BlockType::None);
			}
		}
		// RigidbodyComponentの設定
		else if (compName == "RigidbodyComponent")
		{
			if (auto comp = obj.GetComponent<RigidbodyComponent>())
			{
				std::string typeStr = JsonHelper::GetString(compData, "type");
				if (typeStr == "dynamic") comp->m_type = RigidbodyType::Dynamic;
				else if (typeStr == "kinematic") comp->m_type = RigidbodyType::Kinematic;
				else comp->m_type = RigidbodyType::Static;
			}
		}
		// ColliderComponentの設定
		else if (compName == "ColliderComponent")
		{
			if (auto comp = obj.GetComponent<ColliderComponent>())
			{
				std::string shapeStr = JsonHelper::GetString(compData, "shape");
				std::string modelPath = JsonHelper::GetString(compData, "model");
				auto colModel = modelPath.empty() ? nullptr : KdAssets::Instance().m_modeldatas.GetData(modelPath);

				if (shapeStr == "sphere")
				{
					Math::Vector3 offset = Math::Vector3::Zero;
					Math::Vector3 radius = Math::Vector3::Zero;
					JsonHelper::GetVector3(compData, "offset", offset);
					comp->SetShapeAsShpere(JsonHelper::GetFloat(compData, "radius", 0.5f), offset);
				}
				else if (shapeStr == "box")
				{
					comp->SetShapeAsBoxFromModel(colModel);
				}
				else if (shapeStr == "mesh")
				{
					comp->SetShapeAsMesh(colModel);
				}

				std::string layerStr = JsonHelper::GetString(compData, "layer");
				if (layerStr == "LayerGround") comp->SetLayer(LayerGround);
				else if (layerStr == "LayerBlock") comp->SetLayer(LayerBlock);
				else if (layerStr == "LayerPlayer") comp->SetLayer(LayerPlayer);
			}
		}
		// MovingBlockComponentの設定
		else if (compName == "MovingBlockComponent")
		{
			if (auto comp = obj.GetComponent<MovingBlockComponent>())
			{
				comp->SetActive(JsonHelper::GetBool(compData, "active", false));
			}
		}
	}
}
