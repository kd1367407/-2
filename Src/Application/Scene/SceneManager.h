#pragma once
#include"SceneFader/SceneFader.h"

class BaseScene;

class SceneManager
{
public:
	//モード
	enum class SceneMode
	{
		Game,//ゲームプレイモード
		Create,//オブジェクト編集モード
		UI
	};

	//scene
	enum class SceneType
	{
		Title,
		StageSelect,
		Game,
		Result
	};

	//シングルトン
	static SceneManager& Instance();

	void Init();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void PreDraw();
	void Draw();
	void PostDraw();
	void DrawSprite();
	void Release();

	BaseScene* GetCurrentScene()const { return m_currentScene.get(); }
	SceneMode GetCurrentMode()const { return m_currentMode; }

	void SetMode(SceneMode mode) { m_currentMode = mode; }

	void ChangeScene(SceneType sceneType);

	SceneFader& GetFader() { return m_fader; }

private:
	SceneManager();
	~SceneManager();
	//コピー禁止
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	void ToggleMode();

	//実際の切り替え処理
	void ExecuteSceneChange();

	std::unique_ptr<BaseScene> m_currentScene;
	SceneMode m_currentMode = SceneMode::Create;

	//次に切り替えるシーンを保持
	std::unique_ptr<BaseScene> m_nextScene = nullptr;

	SceneFader m_fader;
	bool m_isChangingScene = false;
};