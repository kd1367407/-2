#pragma once

class GameObject;
class GameScene;
struct EditorGizmoContext;

class Component
{
public:
	friend class GameObject;
	friend class Archetype;//コンポーネント組み立てのため

	virtual ~Component() = default;

	//Jsonから自身の設定を読み込む
	virtual void Configure(const nlohmann::json& data){}

	//自身の状態をJsonオブジェクトに変換
	virtual nlohmann::json ToJson() const { return nlohmann::json(); }

	virtual const char* GetComponentName()const = 0;

	//初期化処理
	virtual void Awake(){}//他のコンポーネントに依存しない準備
	virtual void Start(){}//他のコンポーネントとの連携

	//更新処理
	virtual void PreUpdate(){}
	virtual void Update(){}
	virtual void PostUpdate(){}

	//描画処理
	virtual void PreDraw(){}
	virtual void GenerateDepthMapFromLight() {}
	virtual void DrawLit(){}
	virtual void DrawUnLit(){}
	virtual void DrawBright(){}
	virtual void DrawSprite(){}
	virtual void DrawDebug(){}
	virtual void PostDraw(){}

	virtual void OnInspect() {}
	virtual bool OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene) { return false; }

	GameObject* GetOwner()const { return m_owner; }
	void SetOwner(GameObject* owner) { m_owner = owner; }

	int GetUpdatePriority()const { return m_updatePriority; }

protected:
	//自身の所有者であるGameObjectへのポインタ
	GameObject* m_owner = nullptr;

	//実行優先度(数値が小さいほど先に実行される)
	int m_updatePriority = 0;
private:

};