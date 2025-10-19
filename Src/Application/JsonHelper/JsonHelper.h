#pragma once

class JsonHelper
{
public:
	//Jsonオブジェクトから指定されたキーのVector3を読み込む
	static void GetVector3(const nlohmann::json& jsonObj, const std::string& key, Math::Vector3& outVec, const Math::Vector3& defaultValue = Math::Vector3::Zero)
	{
		if (jsonObj.contains(key) && jsonObj[key].is_array() && jsonObj[key].size() == 3)
		{
			outVec = { jsonObj[key][0], jsonObj[key][1], jsonObj[key][2] };
		}
		else
		{
			outVec = defaultValue;
		}
	}

	//Jsonオブジェクトから指定されたキーの文字列を読み込む
	static std::string GetString(const nlohmann::json& jsonObj, const std::string& key, const std::string& defaultValue = "")
	{
		return jsonObj.value(key, defaultValue);
	}

	//Jsonオブジェクトから指定されたキーのfloatを読み込む
	static float GetFloat(const nlohmann::json& jsonObj, const std::string& key, const float defaultValue = 0.0f)
	{
		return jsonObj.value(key, defaultValue);
	}

	//Jsonオブジェクトから指定されたキーのintを読み込む
	static int GetInt(const nlohmann::json& jsonObj, const std::string& key, const int defaultValue = 0)
	{
		return jsonObj.value(key, defaultValue);
	}

	//Jsonオブジェクトから指定されたキーのboolを読み込む
	static bool GetBool(const nlohmann::json& jsonObj, const std::string& key, const bool defaultValue = false)
	{
		return jsonObj.value(key, defaultValue);
	}
};