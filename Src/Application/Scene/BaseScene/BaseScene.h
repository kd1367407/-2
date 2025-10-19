#pragma once
#include"../Src/Framework/Component/GameObject.h"
#include"../SceneManager.h"

class ICameraComponent;

class BaseScene
{
public:
	BaseScene(){}
	virtual ~BaseScene() {}

	//--Scenemanagerから呼ばれる処理--
	void PreUpdate();
	void Update();
	virtual void PostUpdate();

	void PreDraw();
	virtual void Draw();
	void PostDraw();
	virtual void DrawSprite();

	//--派生クラスでの固有処理--
	virtual void Init() = 0;
	virtual void SceneUpdate() {}
	virtual void Release(){}
	virtual void OnModeChanged(SceneManager::SceneMode newMode){}
	virtual bool HasUnsavedChanges()const { return false; }

	//--ヘルパー関数--
	//GameObject追加
	void AddObject(const std::shared_ptr<GameObject>& obj);

	const std::list<std::shared_ptr<GameObject>>& GetObjList()const { return m_objList; }

	//アクティブカメラを設定
	void SetActiveCamera(const std::shared_ptr<ICameraComponent>& camera);
	//アクティブカメラを取得
	std::shared_ptr<ICameraComponent> GetActiveCamera();

	//指定のオブジェクトを取得
	std::shared_ptr<GameObject> FindObject(std::string objName)const;

protected:
	//シーンが管理する全オブジェクトのリスト
	std::list<std::shared_ptr<GameObject>> m_objList;
	//シーンが持つオブジェクト生成工場
	KdGameObjectFactory m_gameObjectFactory;
	//現在アクティブなカメラへのポインタ
	std::weak_ptr<ICameraComponent> m_wpActiveCamera;
	std::shared_ptr<KdSoundInstance> m_spBGM;
};