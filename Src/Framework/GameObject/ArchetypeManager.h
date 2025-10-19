#pragma once
#include"Archetype.h"

//ゲームに登場する全てのGameObjectの設計図(アーキタイプ)を管理するシングルトン
class ArchetypeManager
{
public:
	//シングルトンインスタンス
	static ArchetypeManager& Instance();

	void Init();

	const Archetype* GetArchetype(const std::string& name)const;

	std::vector<std::string> GetAllArchetypeNames(bool editorOnly = false) const;

private:
	ArchetypeManager() {}
	~ArchetypeManager() {}
	ArchetypeManager(const ArchetypeManager&) = delete;
	ArchetypeManager& operator=(const ArchetypeManager&) = delete;

	void RegisterComponentTypes();

	//JSONデータに基づいてGameObjectのコンポーネントを設定
	void ApplyComponentSettings(GameObject& obj, const nlohmann::json& componentData);

	std::unordered_map<std::string, Archetype> m_archetypes;
	std::map<std::string, std::function<std::shared_ptr<Component>()>> m_componentFactory;
};