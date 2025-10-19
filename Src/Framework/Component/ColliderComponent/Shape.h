#pragma once

//当たり判定形状の基底クラス
class Shape
{
public:
	enum class Type { Sphere, Box, Mesh, Polygon };
	virtual ~Shape() = default;
	virtual Type GetType()const = 0;

	virtual const DirectX::BoundingBox& GetBoundingBox() const = 0;
	void SetBoundingBox(const DirectX::BoundingBox& aabb) { m_aabb = aabb; }

	Math::Vector3 m_offset = Math::Vector3::Zero;

protected:
	DirectX::BoundingBox m_aabb;
};

//球
class SphereShape:public Shape
{
public:
	Type GetType()const override { return Type::Sphere; }
	float m_radius = 0.5f;

	const DirectX::BoundingBox& GetBoundingBox() const override { return m_aabb; }

private:

};

//箱
class BoxShape : public Shape
{
public:
	Type GetType() const override { return Type::Box; }
	Math::Vector3 m_extents = { 0.5f, 0.5f, 0.5f };

	const DirectX::BoundingBox& GetBoundingBox() const override { return m_aabb; }

private:
	
};

//メッシュ
class MeshShape : public Shape
{
public:
	Type GetType() const override { return Type::Mesh; }
	std::shared_ptr<KdModelData> m_spModel = nullptr;

	const DirectX::BoundingBox& GetBoundingBox() const override { return m_aabb; }

private:

};

//ポリゴン
class PolygonShape:public Shape
{
public:
	Type GetType() const override { return Type::Polygon; }

	std::shared_ptr<KdModelData> m_spModel = nullptr;
	const std::vector<Math::Vector3>& GetVertices()const { return m_vertices; }
	const std::vector<KdMeshFace>& GetFaces()const { return m_faces; }

	const DirectX::BoundingBox& GetBoundingBox() const override { return m_aabb; }

private:
	std::vector<Math::Vector3> m_vertices;//頂点
	std::vector<KdMeshFace> m_faces;//面情報
	friend class ColliderComponent;
};