#include "GameObject.h"
#include"Component.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"
#include"../Src/Framework/GameObject/ArchetypeManager.h"
#include"BlockDataComponent/BlockDataComponent.h"
#include"IdComponent/IdComponent.h"
#include"TransformComponent/TransformComponent.h"
#include"RenderComponent/RenderComponent.h"
#include"ColliderComponent/ColliderComponent.h"
#include"MovingBlockComponent/MovingBlockComponent.h"
#include"TransferBlockComponent/TransferBlockComponent.h"
#include"JumpBlockComponent/JumpBlockComponent.h"
#include"RotatingBlockComponent/RotatingBlockComponent.h"
#include"SinkingBlockComponent/SinkingBlockComponent.h"
#include"SlipperyComponent/SlipperyComponent.h"
#include"ScalingBlockComponent/ScalingBlockComponent.h"

void GameObject::Init()
{
	//実行優先度でコンポーネントをソートする
	std::sort(m_component.begin(), m_component.end(), [](const auto& a, const auto& b) {
		return a->GetUpdatePriority() < b->GetUpdatePriority();
		});
	
	for (auto& comp : m_component)
	{
		comp->Awake();
	}

	for (auto& comp : m_component)
	{
		comp->Start();
	}
}

void GameObject::PreUpdate()
{
	for (auto& comp : m_component)
	{
		comp->PreUpdate();
	}
}

void GameObject::Update()
{
	for (auto& comp : m_component)
	{
		comp->Update();
	}
}

void GameObject::PostUpdate()
{
	for (auto& comp : m_component)
	{
		comp->PostUpdate();
	}
}

void GameObject::GenerateDepthMapFromLight()
{
	for (auto& comp : m_component)
	{
		comp->GenerateDepthMapFromLight();
	}
}

void GameObject::PreDraw()
{
	for (auto& comp : m_component)
	{
		comp->PreDraw();
	}
}

void GameObject::DrawLit()
{
	for (auto& comp : m_component)
	{
		comp->DrawLit();
	}
}

void GameObject::DrawUnLit()
{
	for (auto& comp : m_component)
	{
		comp->DrawUnLit();
	}
}

void GameObject::DrawBright()
{
	for (auto& comp : m_component)
	{
		comp->DrawBright();
	}
}

void GameObject::DrawSprite()
{
	for (auto& comp : m_component)
	{
		comp->DrawSprite();
	}
}

void GameObject::PostDraw()
{
	for (auto& comp : m_component)
	{
		comp->PostDraw();
	}
}

void GameObject::DrawDebug()
{
	for (auto& comp : m_component)
	{
		comp->DrawDebug();
	}
	if (!m_pDebugWire)return;

	m_pDebugWire->Draw();
}

void GameObject::AddComponent(const std::shared_ptr<Component>& component)
{
	if (component)
	{
		// 1. 所有者を設定する
		component->m_owner = this;
		// 2. リストに追加する
		m_component.push_back(component);
	}
}

nlohmann::json GameObject::ToJson() const
{
	nlohmann::json entityData;

	//--基本情報--
	std::string objectName = GetName();
	std::string archetypeName = GetArchetypeName();

	entityData["archetype"] = archetypeName;
	entityData["name"] = objectName;
	
	//--各コンポーネントデータ--
	auto& compData = entityData["components"];
	for (auto comp : m_component)
	{
		nlohmann::json compJson = comp->ToJson();
		if (!compJson.empty())
		{
			std::string compName = comp->GetComponentName();
			compData[compName] = compJson;
		}
	}
	return entityData;
}

std::string GameObject::GetArchetypeName() const
{
	std::string name = GetName();
	size_t underbarPos = name.find_last_of('_');
	if (underbarPos != std::string::npos)
	{
		return name.substr(0, underbarPos);
	}

	return name;
}

BlockState GameObject::CreateState() const
{
	BlockState outState;

	//--基本情報--
	outState.archetypeName = GetArchetypeName();
	if (auto idComp = GetComponent<IdComponent>())
	{
		outState.entityId = idComp->GetId();
	}

	const Archetype* archetype = ArchetypeManager::Instance().GetArchetype(outState.archetypeName);
	if (archetype)
	{
		outState.isSwappable = archetype->GetSwapp();
	}

	//--各コンポーネント--
	if (auto comp = GetComponent<TransformComponent>())
	{
		outState.pos = comp->GetPos();
		outState.rot = comp->GetRot();
		outState.scale = comp->GetScale();
	}

	if (auto comp = GetComponent<BlockDataComponent>())
	{
		outState.type = comp->GetType();
	}

	if (auto comp = GetComponent<RenderComponent>())
	{
		outState.renderModelPath = comp->GetModelPath();
	}

	if (auto comp = GetComponent<RigidbodyComponent>())
	{
		outState.hasRigidbody = true;
		outState.rigidbidyType = comp->m_type;
	}

	if (auto comp = GetComponent<ColliderComponent>())
	{
		Shape* shape = comp->GetShape();
		if (shape)
		{
			outState.shapeType = shape->GetType();
			outState.offset = shape->m_offset;
			if (shape->GetType() == Shape::Type::Sphere)
			{
				auto* s = static_cast<SphereShape*>(shape);
				outState.radius = s->m_radius;
			}
			if (shape->GetType() == Shape::Type::Box)
			{
				auto* b = static_cast<BoxShape*>(shape);
				outState.extents = b->m_extents;
				//Boxのモデルパスは現状ColliderComponentにないので空
			}
			if (shape->GetType() == Shape::Type::Mesh)
			{
				auto* m = static_cast<MeshShape*>(shape);
				if (m->m_spModel) outState.collisionModelPath = m->m_spModel->GetFilePath();
			}
			if (shape->GetType() == Shape::Type::Polygon)
			{
				auto* p = static_cast<PolygonShape*>(shape);
				if (p->m_spModel) outState.collisionModelPath = p->m_spModel->GetFilePath();
			}
		}
	}

	if (auto comp = GetComponent<MovingBlockComponent>())
	{
		outState.isMovingBlock = true;
		outState.startPos = comp->GetStartPos();
		outState.endPos = comp->GetEndPos();
		outState.duration = comp->GetDuration();
	}

	if (auto comp = GetComponent<TransferBlockComponent>())
	{
		outState.isTransferBlock = true;
		outState.transferID = comp->GetTransferID();
	}

	if (auto comp = GetComponent<JumpBlockComponent>())
	{
		outState.isJumpBlock = true;
		outState.jumpDirection = comp->GetJumpDirection();
		outState.jumpForce = comp->GetJumpForce();
	}

	if (auto comp = GetComponent<RotatingBlockComponent>())
	{
		outState.isRotatingBlock = true;
		outState.rotationAxis = comp->GetRotatingAxis();
		outState.rotationAmount = comp->GetRotatingAmount();
		outState.rotationSpeed = comp->GetRotatingSpeed();
	}

	if (auto comp = GetComponent<SinkingBlockComponent>())
	{
		outState.isSinkingBlock = true;
		outState.sinkingInitialPos = comp->GetInitialPos();
		outState.maxSinkDistance = comp->GetMaxSinkDistance();
		outState.acceleration = comp->GetAcceleration();
		outState.riseSpeed = comp->GetRiseSpeed();
	}

	if (auto comp = GetComponent<SlipperyComponent>())
	{
		outState.isSlipperyBlock = true;
		outState.slipperyDragCoefficient = comp->GetDragCoefficient();
	}

	if (auto comp = GetComponent<ScalingBlockComponent>())
	{
		outState.isScalingBlock = true;
		outState.scaleAxis = comp->GetScaleAxis();
		outState.scaleAmount = comp->GetscaleAmount();
		outState.scaleSpeed = comp->GetscaleSpeed();
	}

	return outState;
}

void GameObject::ApplyState(const BlockState& state)
{
	if (auto comp = GetComponent<TransformComponent>())
	{
		comp->SetPos(state.pos);
		comp->SetRot(state.rot);
		comp->SetScale(state.scale);
	}

	if (auto comp = GetComponent<BlockDataComponent>())
	{
		comp->SetType(state.type);
	}

	if (auto comp = GetComponent<RenderComponent>())
	{
		auto model = KdAssets::Instance().m_modeldatas.GetData(state.renderModelPath);
		comp->SetModel(model);
	}

	if (auto comp = GetComponent<RigidbodyComponent>())
	{
		comp->m_type = state.rigidbidyType;
		comp->SetVelocity(Math::Vector3::Zero);//状態適用時は速度リセット(吹き飛ばないように)
	}

	if (auto comp = GetComponent<ColliderComponent>())
	{
		switch (state.shapeType)
		{
			case Shape::Type::Sphere:
			{
				comp->SetShapeAsShpere(state.radius, state.offset);
				break;
			}
			case Shape::Type::Box:
			{
				if (!state.collisionModelPath.empty())
				{
					auto colModel = KdAssets::Instance().m_modeldatas.GetData(state.collisionModelPath);
					comp->SetShapeAsBoxFromModel(colModel);
				}
				break;
			}
			case Shape::Type::Mesh:
			{
				if (!state.collisionModelPath.empty())
				{
					auto colModel = KdAssets::Instance().m_modeldatas.GetData(state.collisionModelPath);
					comp->SetShapeAsMesh(colModel);
				}
				break;
			}
			case Shape::Type::Polygon:
			{
				if (!state.collisionModelPath.empty())
				{
					auto colModel = KdAssets::Instance().m_modeldatas.GetData(state.collisionModelPath);
					comp->SetShapeAsPolygon(colModel);
				}
				break;
			}
		}
	}

	if (auto comp = GetComponent<MovingBlockComponent>())
	{
		//ModelのMovingフラグに応じてcomponentを有効化/無効化
		comp->SetActive(state.isMovingBlock);

		if (state.isMovingBlock)
		{
			Math::Vector3 startPos = state.startPos;
			Math::Vector3 endPos = state.endPos;
			float duration = state.duration;

			comp->SetStartPos(startPos);
			comp->SetEndPos(endPos);
			comp->SetDuration(duration);
		}
	}

	if (auto comp = GetComponent<TransferBlockComponent>())
	{
		if (state.isTransferBlock)
		{
			comp->SetTransferID(state.transferID);
		}
	}

	if (auto comp = GetComponent<JumpBlockComponent>())
	{
		if (state.isJumpBlock)
		{
			comp->SetJumpDirection(state.jumpDirection);
			comp->SetJumpForce(state.jumpForce);
		}
	}

	if (auto comp = GetComponent<RotatingBlockComponent>())
	{
		if (state.isRotatingBlock)
		{
			comp->SetRotatingAxis(state.rotationAxis);
			comp->SetRotatingAmount(state.rotationAmount);
			comp->SetRotatingSpeed(state.rotationSpeed);
		}
	}

	if (auto comp = GetComponent<SinkingBlockComponent>())
	{
		if (state.isSinkingBlock)
		{
			comp->SetInitialPos(state.sinkingInitialPos);
			comp->SetMaxSinkDistance(state.maxSinkDistance);
			comp->SetAcceleration(state.acceleration);
			comp->SetRiseSpeed(state.riseSpeed);
		}
	}

	if (auto comp = GetComponent<SlipperyComponent>())
	{
		if (state.isSlipperyBlock)
		{
			comp->SetDragCoefficient(state.slipperyDragCoefficient);
		}
	}

	if (auto comp = GetComponent<ScalingBlockComponent>())
	{
		if (state.isScalingBlock)
		{
			comp->SetScaleAxis(state.scaleAxis);
			comp->SetScaleAmount(state.scaleAmount);
			comp->SetScaleSpeed(state.scaleSpeed);
		}
	}
}

