#pragma once
#include"../Src/Application/GameData/BlockState/BlockState.h"

//型を合わせるため
namespace DirectX::SimpleMath {

	inline void to_json(nlohmann::json& j, const Vector3& v) {
		j = { v.x, v.y, v.z };
	}

	inline void from_json(const nlohmann::json& j, Vector3& v) {
		if (j.is_array() && j.size() == 3) {
			j.at(0).get_to(v.x);
			j.at(1).get_to(v.y);
			j.at(2).get_to(v.z);
		}
	}

}

//--各種enumと文字列の相互変換--
//BlockType
NLOHMANN_JSON_SERIALIZE_ENUM(BlockType, {
	{BlockType::None, "None"},
	{BlockType::Movable, "Movable"},
	{BlockType::Wall, "Wall"},
	{BlockType::Goal, "Goal"},
	{BlockType::Moving, "Moving"},
	{BlockType::Transfer, "Transfer"},
	{BlockType::Slippery, "Slippery"},
	{BlockType::Rotating, "Rotating"},
	{BlockType::Sinking, "Sinking"},
	{BlockType::Scaling, "Scaling"},
	{BlockType::Checkpoint, "Checkpoint"},
	{BlockType::TutorialTrigger, "TutorialTrigger"}
})

//ShapeType
NLOHMANN_JSON_SERIALIZE_ENUM(Shape::Type, {
	{Shape::Type::Sphere, "sphere"},
	{Shape::Type::Box, "box"},
	{Shape::Type::Mesh, "mesh"},
	{Shape::Type::Polygon, "polygon"}
})

//RigidbodyType
NLOHMANN_JSON_SERIALIZE_ENUM(RigidbodyType, {
	{RigidbodyType::Dynamic, "dynamic"},
	{RigidbodyType::Kinematic, "kinematic"},
	{RigidbodyType::Static, "static"}
})

//--BlockStateとJsonの変換ルール--
inline void to_json(nlohmann::json& j, const BlockState& p)
{
	j = nlohmann::json{
		{"archetypeName",p.archetypeName},
		{"entityId",p.entityId},
		{"isSwappable", p.isSwappable}
	};

	//コンポーネントの欄を作成
	auto& components = j["components"];

	//各コンポーネントのデータを入れる
	components["TransformComponent"] = {
		{"position", p.pos},
		{"rotation", p.rot},
		{"scale",    p.scale}
	};

	components["BlockDataComponent"] = {
	   {"type", p.type}
	};

	components["RenderComponent"] = {
	   {"model", p.renderModelPath}
	};

	components["ColliderComponent"] = {
	   {"shape", p.shapeType},
	   {"model", p.collisionModelPath},
	   {"radius", p.radius},
	   {"extents", p.extents},
	   {"offset", p.offset},
	   {"isTrigger", p.isTrigger}
	};

	components["RigidbodyComponent"] = {
		{"hasRigidbody", p.hasRigidbody},
		{"type", p.rigidbidyType}
	};

	if (p.isMovingBlock) {
		components["MovingBlockComponent"] = {
			{"startPos", p.startPos},
			{"endPos",   p.endPos},
			{"duration", p.duration},
			{"active",   p.isMovingBlock}
		};
	}

	if (p.isTransferBlock) {
		components["TransferBlockComponent"] = {
			{"transferID", p.transferID}
		};
	}

	if (p.isJumpBlock)
	{
		components["JumpBlockComponent"] = {
			{"jumpDirection",p.jumpDirection},
			{"jumpForce",p.jumpForce}
		};
	}

	if (p.isRotatingBlock)
	{
		components["RotatingBlockComponent"] = {
	   {"rotation_axis", p.rotationAxis},
	   {"rotation_amount", p.rotationAmount},
	   {"rotation_speed", p.rotationSpeed}
		};
	}

	if (p.isSinkingBlock)
	{
		components["SinkingBlockComponent"] = {
			{"initial_pos", p.sinkingInitialPos},
			{"max_sink_distance", p.maxSinkDistance},
			{"acceleration", p.acceleration},
			{"rise_speed", p.riseSpeed}
		};
	}

	if (p.isSlipperyBlock)
	{
		components["SlipperyComponent"] = {
		{"drag_coefficient", p.slipperyDragCoefficient}
		};
	}

	if (p.isScalingBlock)
	{
		components["ScalingBlockComponent"] = {
			{"scale_axis",p.scaleAxis},
			{"scale_amount",p.scaleAmount},
			{"scale_speed",p.scaleSpeed}
		};
	}

	if (p.hasTutorialTrigger) {
		components["TutorialTriggerComponent"] = {
			{"BlockName", p.tutorialBlockName},
			{"Text", p.tutorialText},
			{"ImagePath", p.tutorialImagePath}
		};
	}

	if (p.isMagicCircle) {
		components["MagicCircleComponent"] = {
			{"model", p.modelPath},
			{"localPos", p.localPos},
			{"localRot", p.localRot},
			{"localScale",    p.localScale}
		};
	}
}

inline void from_json(const nlohmann::json& j, BlockState& p)
{
	p.archetypeName = j.value("archetypeName", "");
	p.entityId = j.value("entityId", 0);
	p.isSwappable = j.value("swappable", false);

	//componentsの欄があるかチェック
	if (j.contains("components"))
	{
		const auto& components = j.at("components");

		if (components.contains("TransformComponent"))
		{
			const auto& data = components.at("TransformComponent");
			p.pos = data.value("position", Math::Vector3::Zero);
			p.rot = data.value("rotation", Math::Vector3::Zero);
			p.scale = data.value("scale", Math::Vector3::One);
		}

		if (components.contains("BlockDataComponent"))
		{
			const auto& data = components.at("BlockDataComponent");
			p.type = data.value("type", BlockType::None);
		}

		if (components.contains("RenderComponent"))
		{
			p.renderModelPath = components.at("RenderComponent").value("model", "");
		}

		if (components.contains("ColliderComponent")) 
		{
			const auto& data = components.at("ColliderComponent");
			p.shapeType = data.value("shape", Shape::Type::Mesh);
			p.collisionModelPath = data.value("model", "");
			p.radius = data.value("radius", 0.5f);
			p.extents = data.value("extents", Math::Vector3(0.5f, 0.5f, 0.5f));
			p.offset = data.value("offset", Math::Vector3::Zero);
			p.isTrigger = data.value("isTrigger", false);
		}

		if (components.contains("RigidbodyComponent")) 
		{
			const auto& data = components.at("RigidbodyComponent");
			p.hasRigidbody = data.value("hasRigidbody", false);
			p.rigidbidyType = data.value("type", RigidbodyType::Static);
		}

		if (components.contains("MovingBlockComponent")) 
		{
			p.isMovingBlock = true; //欄が存在すればフラグを立てる
			const auto& data = components.at("MovingBlockComponent");
			p.startPos = data.value("startPos", Math::Vector3::Zero);
			p.endPos = data.value("endPos", Math::Vector3::Zero);
			p.duration = data.value("duration", 2.0f);
		}

		if (components.contains("TransferBlockComponent")) 
		{
			p.isTransferBlock = true;
			p.transferID = components.at("TransferBlockComponent").value("transferID", 0);
		}

		if (components.contains("SlipperyComponent")) 
		{
			p.isSlipperyBlock = true;
			const auto& data = components.at("SlipperyComponent");
			p.slipperyDragCoefficient = data.value("drag_coefficient", 1.0f);
		}

		if (components.contains("JumpBlockComponent"))
		{
			p.isJumpBlock = true;
			const auto& data = components.at("JumpBlockComponent");
			p.jumpDirection = data.value("jumpDirection", Math::Vector3(0, 1, 0));
			p.jumpForce = data.value("jumpForce", 0.0f);
		}

		if (components.contains("RotatingBlockComponent"))
		{
			p.isRotatingBlock = true;
			const auto& data = components.at("RotatingBlockComponent");
			p.rotationAxis = data.value("rotation_axis", Math::Vector3(0, 1, 0));
			p.rotationAmount = data.value("rotation_amount", 0.0f);
			p.rotationSpeed = data.value("rotation_speed", 0.0f);
		}

		if (components.contains("SinkingBlockComponent"))
		{
			p.isSinkingBlock = true;
			const auto& data = components.at("SinkingBlockComponent");
			p.sinkingInitialPos = data.value("initial_pos", Math::Vector3::Zero);
			p.maxSinkDistance = data.value("max_sink_distance", 0.0f);
			p.acceleration = data.value("acceleration", 0.0f);
			p.riseSpeed = data.value("rise_speed", 0.0f);
		}

		if (components.contains("ScalingBlockComponent"))
		{
			p.isScalingBlock = true;
			const auto& data = components.at("ScalingBlockComponent");
			p.scaleAxis = data.value("scale_axis", Math::Vector3{ 0.0,1.0,0.0 });
			p.scaleAmount = data.value("scale_amount", 0.5);
			p.scaleSpeed = data.value("scale_speed", 2.0);
		}

		if (components.contains("TutorialTriggerComponent"))
		{
			p.hasTutorialTrigger = true;
			const auto& data = components.at("TutorialTriggerComponent");
			p.tutorialBlockName = data.value("BlockName", "");
			p.tutorialText = data.value("Text", "");
			p.tutorialImagePath = data.value("ImagePath", "");
		}

		if (components.contains("MagicCircleComponent"))
		{
			p.isMagicCircle = true;
			const auto& data = components.at("MagicCircleComponent");
			p.localPos = data.value("localPos", Math::Vector3::Zero);
			p.localRot = data.value("localRot", Math::Vector3::Zero);
			p.localScale = data.value("localScale", Math::Vector3::One);
		}
	}
}