#include "KdGameObjectFactory.h"

void KdGameObjectFactory::RegisterCreateFunction(const std::string_view str, const std::function<std::shared_ptr<KdGameObject>(void)> func)
{
	m_createFunctions[str.data()] = func;
}

std::shared_ptr<KdGameObject> KdGameObjectFactory::CreateGameObject(const std::string_view objName)
{
	auto creater = m_createFunctions.find(objName);

	if (creater == m_createFunctions.end())
	{
		assert(0 && "GameObjectFactoryに未登録のゲームオブジェクトクラスです");
		return nullptr;
	}

	//登録された関数でオブジェクト生成
	std::shared_ptr<KdGameObject> spObj = creater->second();

	if (spObj)
	{
		//objectの初期化関数呼び出し
		spObj->Init();
	}

	return spObj;
}



const std::vector<std::string> KdGameObjectFactory::GetCreatableObjectNames() const
{
	std::vector<std::string> names;
	//メモリ事前確保
	names.reserve(m_createFunctions.size());
	for (const auto& pair : m_createFunctions)
	{
		names.push_back(pair.first.data());
	}

	return names;
}
