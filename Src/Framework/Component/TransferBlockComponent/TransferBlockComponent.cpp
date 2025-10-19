#include "TransferBlockComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../RigidbodyComponent/RigidbodyComponent.h"
#include"../BlockDataComponent/BlockDataComponent.h"
#include"../ColliderComponent/ColliderComponent.h"
#include"../ColliderComponent/Shape.h"
#include"../Src/Application/main.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void TransferBlockComponent::Awake()
{
}

void TransferBlockComponent::Start()
{
	if (auto dataComp = m_owner->GetComponent<BlockDataComponent>(); !dataComp)
	{
		auto newDataComp = m_owner->GetComponent<BlockDataComponent>();
		newDataComp->SetType(BlockType::Transfer);
	}
}

void TransferBlockComponent::Update()
{
	if (m_coolDown > 0.0f)
	{
		m_coolDown -= Application::Instance().GetDeltaTime();
	}
}

void TransferBlockComponent::OnCollision(const CollisionInfo& info)
{
	//クールダウン中、衝突相手がいない、パートナーがいない場合は処理しない
	if (m_coolDown > 0.0f || !info.otherObject || m_wpPartner.expired()) return;

	//法線がほぼ真上を向いているか
	if (info.contactNormal.y > 0.7f)
	{
		//dynamicなrigidbodyを持っているか
		if (auto rigid = info.otherObject->GetComponent<RigidbodyComponent>())
		{
			if (rigid->m_type == RigidbodyType::Dynamic)
			{
				if (auto partner = m_wpPartner.lock())
				{
					auto partnerTransform = partner->GetComponent<TransformComponent>();
					auto partnerCollider = partner->GetComponent<ColliderComponent>();
					if (!partnerTransform || !partnerCollider)return;

					//パートナーブロックの上面の座標を計算

					float verticalOffset = 0.0f;//Y軸方向のオフセット
					Shape* partnerShape = partnerCollider->GetShape();

					if (partnerShape)//将来当たり判定形状を変えたとき用
					{
						//パートナーのコライダーの形状に応じて高さ計算
						switch (partnerShape->GetType())
						{
							case Shape::Type::Box:
							{
								auto* box = static_cast<BoxShape*>(partnerShape);
								verticalOffset = box->m_extents.y * 2.0f;
								break;
							}
							case Shape::Type::Sphere:
							{
								auto* sphere = static_cast<SphereShape*>(partnerShape);
								verticalOffset = sphere->m_offset.y + sphere->m_radius;
								break;
							}
							case Shape::Type::Mesh:
							{
								auto* mesh = static_cast<MeshShape*>(partnerShape);
								if (mesh->m_spModel)
								{
									//メッシュのAABBから高さを計算
									const auto& aabb = mesh->m_spModel->GetMesh(0)->GetBoundingBox();
									verticalOffset = aabb.Center.y + aabb.Extents.y;
								}
								break;
							}
							case Shape::Type::Polygon:
							{
								auto* polygon = static_cast<PolygonShape*>(partnerShape);

								const auto& aabb = polygon->GetBoundingBox();
								verticalOffset = aabb.Extents.y + aabb.Center.y;
								break;
							}
						}
					}

					//テレポート先の座標計算(ブロックの少し上)
					Math::Vector3 teleportPos = partnerTransform->GetPos();
					teleportPos.y += (verticalOffset * partnerTransform->GetScale().y) + 0.1f; // スケールを考慮し、埋まらないように少し浮かせる

					//テレポートするオブジェクトの座標更新
					info.otherObject->GetComponent<TransformComponent>()->SetPos(teleportPos);
					//速度リセット
					rigid->m_velocity = Math::Vector3::Zero;

					Application::Instance().AddLog("Teleport Object");

					//両方の転移ブロックをクールタイム状態に
					m_coolDown = 2.0f;
					if (auto partnerComp = partner->GetComponent<TransferBlockComponent>())
					{
						partnerComp->m_coolDown = 2.0f;
					}
				}
			}
		}
	}
}

void TransferBlockComponent::OnInspect()
{
	if (ImGui::CollapsingHeader("Transform Compoent", ImGuiTreeNodeFlags_DefaultOpen))
	{
		int id = m_transfarID;
		if (ImGui::InputInt("TransferID", &id))
		{
			m_transfarID = id;
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			if (auto spViewModel = m_wpViewModel.lock())
			{
				spViewModel->PairTransferBlocks();
			}
		}

		std::string partnerName = "None";
		if (auto spPartner = m_wpPartner.lock())
		{
			partnerName = spPartner->GetName();
		}
		ImGui::InputText("Partner", &partnerName[0], partnerName.size(), ImGuiInputTextFlags_ReadOnly);
	}
}

void TransferBlockComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("TransferBlockComponent"))return;

	const auto& transferBlockData = data.at("TransferBlockComponent");

	m_transfarID = JsonHelper::GetInt(transferBlockData, "transferID", 0);
}

nlohmann::json TransferBlockComponent::ToJson() const
{
	nlohmann::json j;
	j["transferID"] = m_transfarID;

	return j;
}
