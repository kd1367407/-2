#pragma once
#include"../Component/ColliderComponent/Shape.h"
#include"../Component/RigidbodyComponent/RigidbodyComponent.h"

class GameObject;
class Component;

//GameObjectの[設計図]となるクラス
class Archetype
{
public:
	//この設計図から新しいGameObjectインスタンスを生成
	std::shared_ptr<GameObject> Instantiate(const nlohmann::json& entityData)const;

	//この設計図にコンポーネントを追加する
	template<class T>
	void AddComponent()
	{
		//TがComponentを継承していることをコンパイル時にチェック
		static_assert(std::is_base_of<Component,T>::value, "T must be a descendant of Component");

		//コンポーネントを生成する[関数]をリストに保存
		m_componentCreators.push_back([]() {
			return std::make_shared<T>();
		});
	}

	//設定用の関数を登録する
	void SetConfigurer(const std::function<void(GameObject&)>& configFunc)
	{
		m_configuer = configFunc;
	}

	void SetIsSpwawnInEditor(bool flg) { m_isSpawnableInEditor = flg; }
	const bool& GetSpawnInEditor()const { return m_isSpawnableInEditor; }

	void SetIsSwapp(bool flg) { m_isSwappable = flg; }
	const bool& GetSwapp()const { return m_isSwappable; }

	bool IsSavable() const { return m_isSavable; }

	const std::string& GetDefaultRenderModelPath()const { return m_defaultRenderModelPath; }
	const std::string& GetDefaultName()const { return m_defaultName; }
	Shape::Type GetDefaultShapeType()const { return m_defaultShapeType; }
	const std::string& GetDefaultCollisionModelPath()const { return m_defaultColModelPath; }
	RigidbodyType GetDefaultRigidbodyType()const { return m_defaultRigidbodyType; }
	const nlohmann::json& GetComponentData() const { return m_componentData; }

private:
	friend class GameObjectFactory;
	friend class ArchetypeManager;

	//コンポーネントを生成するための関数のリスト
	std::vector<std::function<std::shared_ptr<Component>()>> m_componentCreators;

	//設定用関数
	std::function<void(GameObject&)> m_configuer;

	//生成リストに表示するかどうか
	bool m_isSpawnableInEditor = true;

	bool m_isSwappable = false;

	bool m_isSavable = true;

	//デフォルト値
	std::string m_defaultRenderModelPath;
	std::string m_defaultName;
	Shape::Type m_defaultShapeType = Shape::Type::Mesh;
	std::string m_defaultColModelPath;
	RigidbodyType m_defaultRigidbodyType = RigidbodyType::Static;

	//コンポーネントごとのデフォルト値を保持
	nlohmann::json m_componentData;
};