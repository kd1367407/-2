#include "GameObjectFactory.h"
#include"ArchetypeManager.h"
#include"../Component/GameObject.h"

std::shared_ptr<GameObject> GameObjectFactory::CreateGameObject(const std::string archetypeName, const nlohmann::json& entityData)
{
	const Archetype* archetype = ArchetypeManager::Instance().GetArchetype(archetypeName);
	
	if (!archetype)return nullptr;

	return archetype->Instantiate(entityData);
}
