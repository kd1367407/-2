#include "StageListManager.h"
#include"../JsonHelper/JsonHelper.h"

void StageListManager::UpdateStageLabel(const std::string& filePath, const std::string& newLabel)
{
	if (m_stageListData.contains("stages") && m_stageListData["stages"].is_array())
	{
		for (auto& stageInfo : m_stageListData["stages"])
		{
			std::string path = JsonHelper::GetString(stageInfo, "path");
			if (path == filePath)
			{
				stageInfo["label"] = newLabel;
				Save();
				break;
			}
		}
	}
}

void StageListManager::AddStageEntry(const std::string& newLabel, const std::string& newFilePath)
{
	if (!m_stageListData.contains("stages"))
	{
		m_stageListData["stages"] = nlohmann::json::array();
	}

	nlohmann::json newEntry;
	newEntry["label"] = newLabel;
	newEntry["path"] = newFilePath;

	m_stageListData["stages"].push_back(newEntry);
	Save();
}

std::string StageListManager::GenerateNewStagePath()
{
	int stageNumber = 0;
	std::string newFilePath;

	while (true)
	{
		//"Stage" + 2桁の数字(0埋め) + ".json" を生成
		std::string filePath = std::format("Stage{:02}.json", stageNumber);
		newFilePath = "Asset/Data/Stages/" + filePath;

		if (!StagePathExists(newFilePath))
		{
			break;//このパスは使われていないので決定
		}
		stageNumber++;
	}
	return newFilePath;
}

StageListManager::StageListManager()
{
	Load();
}

void StageListManager::Load()
{
	std::ifstream ifs(m_filePath);
	if (ifs.is_open())
	{
		ifs >> m_stageListData;
	}
	else
	{
		m_stageListData = nlohmann::json::object();
		m_stageListData["stages"] = nlohmann::json::array();
	}
}

void StageListManager::Save()
{
	std::ofstream ofs(m_filePath);

	if (ofs)
	{
		ofs << std::setw(4) << m_stageListData << std::endl;
	}
}

bool StageListManager::StagePathExists(const std::string& filePath)
{
	if (m_stageListData.contains("stages") && m_stageListData["stages"].is_array())
	{
		for (const auto& stageInfo : m_stageListData["stages"])
		{
			std::string path = JsonHelper::GetString(stageInfo, "path");
			if (path == filePath)
			{
				return true;//存在した
			}
		}
	}

	return false;//存在しなかった
}
