#pragma once
#include"Component.h"//ダウンキャストやメモリのサイズの為

struct BlockState;

//コンポーネントを保持する[箱]となるクラス
class GameObject:public std::enable_shared_from_this<GameObject>
{
public:

	friend class Archetype;

	void Init();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void GenerateDepthMapFromLight();
	void PreDraw();
	void DrawLit();
	void DrawUnLit();
	void DrawBright();
	void DrawSprite();
	void PostDraw();
	void DrawDebug();


	//コンポーネントを新たに生成して自身に追加
	template<class T>
	std::shared_ptr<T> AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must be a descendant of Component");
		auto newComp = std::make_shared<T>();
		newComp->m_owner = this;
		m_component.push_back(newComp);
		return newComp;
	}

	//生成済みのコンポーネントを追加する
	void AddComponent(const std::shared_ptr<Component>& component);

	//自身が持つ特定のコンポーネントを取得
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		for (auto& comp : m_component)
		{
			if (auto casted = std::dynamic_pointer_cast<T>(comp))
			{
				return casted;
			}
		}
		return nullptr;
	}

	template<class T>
	std::shared_ptr<const T> GetComponent() const
	{
		for (auto& comp : m_component)
		{
			if (auto casted = std::dynamic_pointer_cast<const T>(comp))
			{
				return casted;
			}
		}
		return nullptr;
	}

	//自身が持つコンポーネントリストを取得
	const std::vector<std::shared_ptr<Component>>& GetComponents()const { return m_component; }

	bool IsExpired()const { return m_isExpired; }

	void Expired() { m_isExpired = true; }

	const std::string& GetName()const
	{
		return m_name;
	}

	void SetName(const std::string& name)
	{
		m_name = name;
	}

	nlohmann::json ToJson() const;

	std::string GetArchetypeName()const;

	void ApplyState(const BlockState& state);
	BlockState CreateState()const;


protected:
	

private:
	std::string m_name = "GameObjects";
	std::vector<std::shared_ptr<Component>> m_component;
	bool m_isExpired = false;
	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;
};