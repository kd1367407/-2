#include "SettingsManager.h"
#include"../main.h"

SettingsManager& SettingsManager::Instance()
{
	static SettingsManager instance;
	return instance;
}

void SettingsManager::Init()
{
	LoadJson("Asset/Data/Settings/GameSettings.json", m_gameSettings);
	LoadJson("Asset/Data/Settings/Input.json", m_inputSettings);
}

void SettingsManager::SaveGameSetting()
{
	SaveJson("Asset/Data/Settings/GameSettings.json", m_gameSettings);
}

void SettingsManager::LoadJson(const std::string& filepath, nlohmann::json& outJson)
{
	std::ifstream ifs(filepath);

	if (ifs.is_open())
	{
		try
		{
			ifs >> outJson;
		}
		catch (nlohmann::json::parse_error& e)
		{
			std::string errorMsg = "JSON parse error at " + filepath + ": " + e.what();
			Application::Instance().AddLog(errorMsg.c_str());
		}
	}
	else
	{
		std::string errorMsg = "Failed to open JSON file: " + filepath;
		Application::Instance().AddLog(errorMsg.c_str());
	}
}

void SettingsManager::SaveJson(const std::string& filepath, const nlohmann::json& json)
{
	std::ofstream ofs(filepath);
	if (ofs.is_open())
	{
		ofs << std::setw(4) << json << std::endl;
	}
	else
	{
		std::string errorMsg = "Failed to open JSON file for writing: " + filepath;
		Application::Instance().AddLog(errorMsg.c_str());
	}
}
