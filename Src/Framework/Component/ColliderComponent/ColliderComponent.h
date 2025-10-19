#pragma once
#include"../Component.h"
#include"Shape.h"

enum CollisionLayer :uint32_t
{
	LayerGround = 1 << 0,//地面
	LayerBlock = 1 << 1,//ブロック
	LayerPlayer = 1 << 2,//プレイヤー

	LayerAll = 0xFFFFFFFF//全て
};

class TransformComponent;

//当たり判定の形状を定義する基底クラス
class ColliderComponent :public Component, public std::enable_shared_from_this<ColliderComponent>
{
public:
	void Awake()override;
	void Start()override;
	void Configure(const nlohmann::json& data)override;
	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "ColliderComponent"; }

	//このColliderが所属するレイヤー
	uint32_t GetLayer() const { return m_layer; }
	void SetLayer(uint32_t layer) { m_layer = layer; }

	//形状登録
	template<typename T>
	void SetShape(T shape) { m_shape = shape; }

	//球形状設定(player用)
	void SetShapeAsShpere(float radius, const Math::Vector3& offset = Math::Vector3::Zero);

	//モデルから自動で箱の大きさを設定
	void SetShapeAsBoxFromModel(const std::shared_ptr<KdModelData>& model);

	//メッシュ形状設定(動かないオブジェクト用)
	void SetShapeAsMesh(const std::shared_ptr<KdModelData>& model);

	//ポリゴン形状設定
	void SetShapeAsPolygon(const std::shared_ptr<KdModelData>& model);

	//PhysicsSystemが使うためのゲッター
	Shape* GetShape()const { return m_shape.get(); }

	//トリガー系
	bool isTrigger()const { return m_isTrigger; }
	void SetTrigger(bool isTrigger) { m_isTrigger = isTrigger; }

	const std::shared_ptr<TransformComponent>& GetTransform()const { return m_transform; }

	void OnInspect()override;

private:
	std::unique_ptr<Shape> m_shape;
	uint32_t m_layer = 0;
	std::shared_ptr<TransformComponent> m_transform;
	std::string m_modelPath;

	//トリガーフラグ
	bool m_isTrigger = false;
};