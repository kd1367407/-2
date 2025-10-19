#include "ColliderComponent.h"
#include"../Src/Application/System/PhysicsSystem.h"
#include"Shape.h"
#include"../TransformComponent/TransformComponent.h"
#include"../GameObject.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void ColliderComponent::Awake()
{
}

void ColliderComponent::Start()
{
	m_transform = m_owner->GetComponent<TransformComponent>();
	PhysicsSystem::Instance().RegisterCollider(std::dynamic_pointer_cast<ColliderComponent>(shared_from_this()));
}

void ColliderComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("ColliderComponent"))return;

	const auto& colData = data.at("ColliderComponent");

	m_isTrigger = JsonHelper::GetBool(colData, "isTrigger", false);

	//レイヤー設定
	if (colData.contains("layer"))
	{
		uint32_t finalLayer = 0;

		//Jsonの値が単一の文字列か文字列の配列かチェック
		if (colData["layer"].is_string())
		{
			std::string layerStr = colData["layer"].get<std::string>();
			if (layerStr == "LayerGround")
			{
				finalLayer |= CollisionLayer::LayerGround;
			}
			else if (layerStr == "LayerBlock")
			{
				finalLayer |= CollisionLayer::LayerBlock;
			}
			else if (layerStr == "LayerPlayer")
			{
				finalLayer |= CollisionLayer::LayerPlayer;
			}
		}
		else if (colData["layer"].is_array())
		{
			for (const auto& layerName : colData["layer"])
			{
				std::string layerStr = layerName.get<std::string>();
				if (layerStr == "LayerGround")
				{
					finalLayer |= CollisionLayer::LayerGround;
				}
				else if (layerStr == "LayerBlock")
				{
					finalLayer |= CollisionLayer::LayerBlock;
				}
				else if (layerStr == "LayerPlayer")
				{
					finalLayer |= CollisionLayer::LayerPlayer;
				}
			}
		}
		SetLayer(finalLayer);
	}

	//当たり判定形状設定
	if (colData.contains("shape"))
	{
		std::string shapeStr = colData["shape"].get<std::string>();

		//当たり判定モデル設定
		if (colData.contains("model"))
		{
			m_modelPath = JsonHelper::GetString(colData, "model");
			auto colModel = m_modelPath.empty() ? nullptr : KdAssets::Instance().m_modeldatas.GetData(m_modelPath);
		}

		if (shapeStr == "sphere")
		{
			Math::Vector3 offset = {};
			float radius = 0;
			JsonHelper::GetVector3(colData, "offset", offset);
			radius = JsonHelper::GetFloat(colData, "radius", 0.5f);
			SetShapeAsShpere(radius, offset);
		}
		else if (shapeStr == "box" || shapeStr == "mesh"||shapeStr=="polygon")
		{
			if (colData.contains("model"))
			{
				std::string modelPath = colData.at("model").get<std::string>();
				auto colModel = modelPath.empty() ? nullptr : KdAssets::Instance().m_modeldatas.GetData(modelPath);

				if (shapeStr == "box")
				{
					SetShapeAsBoxFromModel(colModel);
				}
				else if (shapeStr == "polygon")
				{
					SetShapeAsPolygon(colModel);
				}
				else
				{
					SetShapeAsMesh(colModel);
				}
			}
		}
		
	}
}

nlohmann::json ColliderComponent::ToJson() const
{
	nlohmann::json j;
	std::string layer = "";
	std::vector<std::string> layerNames;

	if (m_layer & LayerGround)
	{
		layerNames.push_back("LayerGround");
	}
	if (m_layer & LayerBlock)
	{
		layerNames.push_back("LayerBlock");
	}
	if (m_layer & LayerPlayer)
	{
		layerNames.push_back("LayerPlayer");
	}
	if (!layerNames.empty())
	{
		if (layerNames.size() == 1)
		{
			j["layer"] = layerNames.front();
		}
		else
		{
			j["layer"] = layerNames;
		}
	}

	if (m_isTrigger)
	{
		j["isTrigger"] = true;
	}


	if (m_shape)
	{
		switch (m_shape->GetType())
		{
			case Shape::Type::Sphere:
			{
				j["shape"] = "shpere";
				auto* s = static_cast<SphereShape*>(m_shape.get());
				j["radius"] = s->m_radius;
				j["offset"] = { s->m_offset.x,s->m_offset.y,s->m_offset.z };
				break;
			}
			case Shape::Type::Box:
			{
				j["shape"] = "box";
				j["model"] = m_modelPath;
				break;
			}
			case Shape::Type::Mesh:
			{
				j["shape"] = "mesh";
				j["model"] = m_modelPath;
				break;
			}
		}
	}

	return j;
}

void ColliderComponent::SetShapeAsShpere(float radius, const Math::Vector3& offset)
{
	auto sphere = std::make_unique<SphereShape>();
	sphere->m_radius = radius;
	sphere->m_offset = offset;

	sphere->SetBoundingBox(
		DirectX::BoundingBox(
			{ offset.x, offset.y, offset.z },
			{ radius, radius, radius }
		)
	);

	m_shape = std::move(sphere);
}

void ColliderComponent::SetShapeAsBoxFromModel(const std::shared_ptr<KdModelData>& model)
{
	if (!model || !model->GetMesh(0))return;

	//モデルのバウンディングボックスから大きさを取得
	const auto& aabb = model->GetMesh(0)->GetBoundingBox();

	//BoxShapeを生成
	auto box = std::make_unique<BoxShape>();
	box->m_extents = aabb.Extents;
	box->m_offset = aabb.Center;//BoundingBoxの中心をオフセットとして保存

	box->SetBoundingBox(aabb);

	m_shape = std::move(box);
}

void ColliderComponent::SetShapeAsMesh(const std::shared_ptr<KdModelData>& model)
{
	auto mesh = std::make_unique<MeshShape>();
	mesh->m_spModel = model;
	m_shape = std::move(mesh);
}

void ColliderComponent::SetShapeAsPolygon(const std::shared_ptr<KdModelData>& model)
{
	auto polygon = std::make_unique<PolygonShape>();
	polygon->m_spModel = model;
	auto vertices = polygon->m_spModel->GetMesh(0)->GetVertexPositions();
	auto faces = polygon->m_spModel->GetMesh(0)->GetFaces();
	polygon->m_vertices = vertices;
	polygon->m_faces = faces;
	DirectX::BoundingBox::CreateFromPoints(polygon->m_aabb, polygon->m_vertices.size(), &polygon->m_vertices[0], sizeof(Math::Vector3));
	m_shape = std::move(polygon);
}

void ColliderComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Collider Component", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//レイヤー情報表示
		ImGui::Text("Layer: %u", m_layer);

		//形状情報表示
		if (m_shape)
		{
			std::string shapeTypeStr;
			switch (m_shape->GetType())
			{
			case Shape::Type::Sphere:
				shapeTypeStr = "Sphere";
				break;
			case Shape::Type::Box:
				shapeTypeStr = "Box";
				break;
			case Shape::Type::Mesh:
				shapeTypeStr = "Mesh";
				break;
			case Shape::Type::Polygon:
				shapeTypeStr = "Polygon";
				break;
			default:
				shapeTypeStr = "Unknown";
				break;
			}
			ImGui::Text("Shape Type: %s", shapeTypeStr.c_str());

			//球なら半径、箱なら大きさを表示
			if (m_shape->GetType() == Shape::Type::Sphere)
			{
				auto* sphere = static_cast<SphereShape*>(m_shape.get());
				ImGui::DragFloat("Radius", &sphere->m_radius, 0.01f);
			}
			else if (m_shape->GetType() == Shape::Type::Box)
			{
				auto* box = static_cast<BoxShape*>(m_shape.get());
				ImGui::InputFloat3("Extents", &box->m_extents.x, "%.3f", ImGuiInputTextFlags_ReadOnly);
			}
		}
	}
}
