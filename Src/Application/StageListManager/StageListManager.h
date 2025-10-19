#pragma once

class StageListManager
{
public:
	static StageListManager& Instance()
	{
		static StageListManager instance;
		return instance;
	}

	//ステージのラベル更新
	void UpdateStageLabel(const std::string& filePath, const std::string& newLabel);

	//新しいステージをリストに追加
	void AddStageEntry(const std::string& newLabel, const std::string& newFilePath);

	//ユニークなファイルパスを作成
	std::string GenerateNewStagePath();

	const nlohmann::json& GetStageListData()const { return m_stageListData; }


private:
	StageListManager();
	~StageListManager() = default;
	StageListManager(const StageListManager&) = delete;
	StageListManager& operator=(const StageListManager&) = delete;

	void Load();
	void Save();
	//指定されたパスがリストに存在するか
	bool StagePathExists(const std::string& filePath);

	nlohmann::json m_stageListData;
	const std::string m_filePath = "Asset/Data/Stages/StageList.json";
};