#pragma once

class KdGameObject;

class KdGameObjectFactory
{
public:

	KdGameObjectFactory() {}
	~KdGameObjectFactory() {}

	//ファクトリにクラスの生成方法を登録する
	void RegisterCreateFunction(const std::string_view name, const std::function <std::shared_ptr<KdGameObject>(void)> func);

	template<class T>
	std::shared_ptr<T> CreateGameObject()
	{
		auto spObj = std::make_shared<T>();
		spObj->Init();
		return spObj;
	}

	//文字列からGameObjectを生成
	std::shared_ptr<KdGameObject> CreateGameObject(const std::string_view objName);

	//登録されているクラス名の一覧を取得
	const std::vector<std::string> GetCreatableObjectNames()const;

private:

	// GameObjectのインスタンスリスト
	std::list<std::shared_ptr<KdGameObject>> m_objects;

	// GameObjectの生成関数
	std::unordered_map<std::string_view, std::function<std::shared_ptr<KdGameObject>(void)>> m_createFunctions;
};