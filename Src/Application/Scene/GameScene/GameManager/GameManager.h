#pragma once

class GameManager
{
public:
	enum class LoadMode
	{
		Play,//ゲームプレイ
		Edit,//既存のステージの編集
		CreateNew//新規ステージの作成
	};

	static GameManager& Instance() 
	{
		static GameManager instance;
		return instance;
	}

	//次に読み込むステージのパス設定
	void SetNextStage(const std::string& filepath,const std::string& label) 
	{
		m_currentStagePath = filepath; 
		m_currentStageLabel = label;
	}

	//現在読み込んでいるステージののファイルパス取得
	const std::string& GetCurrentFilePath()const { return m_currentStagePath; }

	const std::string& GetCurrentStageLabel()const { return m_currentStageLabel; }
	void SetCurrentStageLabel(const std::string& label) { m_currentStageLabel = label; }

	void SetFinalTime(float time) { m_finalTime = time; }
	float GetFinalTime()const { return m_finalTime; }

	void SetLoadMode(LoadMode mode) { m_loadMode = mode; }
	LoadMode GetLoadMode()const { return m_loadMode; }

	//新規ステージ保存後に自身の状態を更新
	void UpdateAfterNewStageSave(const std::string& newFilePath);

private:
	GameManager() = default;
	~GameManager() = default;

	//デフォルト値を設定しておく
	std::string m_currentStagePath= "Asset/Data/Stages/Stage01.json";
	LoadMode m_loadMode = LoadMode::Play;
	std::string m_currentStageLabel = "Default Stage";
	
	//ゴールタイム
	float m_finalTime = 0.0f;
};