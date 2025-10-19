#pragma once

class GameObject;
class Archetype;

//アーキタイプを管理し、GameObjectを生成する工場
class GameObjectFactory
{
public:
	GameObjectFactory() = default;
	~GameObjectFactory() = default;

	//設計図（アーキタイプ）を受け取り、新しいGameObjectを生成する(個別データは任意)
	std::shared_ptr<GameObject> CreateGameObject(const std::string archetypeName, const nlohmann::json& entityData = {});

private:
	
};