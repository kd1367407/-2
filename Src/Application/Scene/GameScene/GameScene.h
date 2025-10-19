#pragma once
#include"../BaseScene/BaseScene.h"
#include"../Src/Framework/Editor/Editor.h"
#include"../../Effect/Particle/ParticleSystem.h"

class InputHandler;
class CommandInvoker;
class StageModel;
class GameObject;
class GameViewModel;
class EditorCameraComponent;

class GameScene:public BaseScene
{
public:
	GameScene(){}
	~GameScene()override { }

	void Init()override;
	//シーンの固有更新
	void SceneUpdate()override;
	void PostUpdate()override;

	void Draw()override;
	void Release()override;

	void OnModeChanged(SceneManager::SceneMode newMode)override;

	bool HasUnsavedChanges()const override;

	//シーン保存依頼
	void RequestSaveStage();

	//アンドゥ処理依頼
	void Undo();

	//アンドゥ履歴リセット依頼
	void UndoClearFrimEditor();

	//StageListの更新依頼
	void UpdateStageListLabel();


	std::shared_ptr<GameViewModel> GetViewModel() { return m_spGameViewModel; }

	//エディタモードカメラ取得
	std::shared_ptr<EditorCameraComponent> GetEditorCamera()const;

	std::shared_ptr<KdDebugWireFrame> GetDebugWire()const { return m_debugWire; }

	void ShowTutorialHint(const std::string& text, const std::string& imagePath, const std::string& blockName);

	ParticleSystem* GetParticleSystem() { return &m_particleSystem; }

private:
	void DrawControlsWindow();
	void DrawTutorialHintWindow();

	//ViewModel
	std::shared_ptr<GameViewModel> m_spGameViewModel;

	//シーンが管理するシステムと主要なオブジェクトへのポインタ
	std::shared_ptr<CommandInvoker> m_commandInvoker;
	std::shared_ptr<InputHandler> m_gameInputHandler;
	std::shared_ptr<StageModel> m_spStageModel;
	std::shared_ptr<KdDebugWireFrame> m_debugWire;
	
	bool m_bShowControlsWindow = false;

	bool m_bShowTutorialHint = false;
	std::string m_blockName;
	std::string m_hitText;
	std::shared_ptr<KdTexture> m_pHintTexture = nullptr;

	Editor m_editor;

	ParticleSystem m_particleSystem;
};