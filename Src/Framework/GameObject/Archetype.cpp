#include "Archetype.h"
#include"../Component/GameObject.h"
#include"../Component/Component.h"
#include"../Component/TransformComponent/TransformComponent.h"
#include"../Component/PlayerStatsComponent/PlayerStatsComponent.h"

std::shared_ptr<GameObject> Archetype::Instantiate(const nlohmann::json& entityData) const
{
	//--骨格生成--
	auto newObject = std::make_shared<GameObject>();
	for (const auto& creator : m_componentCreators)
	{
		newObject->AddComponent(creator());
	}
	newObject->SetName(m_defaultName);

	//--デフォルト値適用--
	for (const auto& comp : newObject->GetComponents())
	{
		comp->Configure(m_componentData);
	}

	//--ステージ個別情報で上書き--
	if (entityData.contains("components"))
	{
		for (const auto& comp : newObject->GetComponents())
		{
			comp->Configure(entityData.at("components"));
		}
	}

	if (entityData.contains("name"))
	{
		newObject->SetName(entityData.at("name").get<std::string>());
	}

	newObject->Init();

	return newObject;
}
