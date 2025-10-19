#pragma once

class SettingsManager
{
public:
	static SettingsManager& Instance();

	void Init();

	const nlohmann::json& GetGameSetting()const { return m_gameSettings; }
	const nlohmann::json& GetInputSetting()const { return m_inputSettings; }

	nlohmann::json& WorkGameSetting() { return m_gameSettings; }

	void SaveGameSetting();

private:
	SettingsManager() = default;
	~SettingsManager() = default;
	SettingsManager(const SettingsManager&) = delete;
	SettingsManager& operator=(const SettingsManager&) = delete;

	void LoadJson(const std::string& filepath, nlohmann::json& outJson);
	void SaveJson(const std::string& filepath, const nlohmann::json& json);

	nlohmann::json m_gameSettings;
	nlohmann::json m_inputSettings;
};