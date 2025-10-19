#include "BlockDataComponent.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void BlockDataComponent::Configure(const nlohmann::json& data)
{
	if (data.is_null() || !data.contains("BlockDataComponent"))return;

	const auto& blockData = data.at("BlockDataComponent");

	//レイヤー設定
	if (blockData.contains("type"))
	{
		std::string typeStr = JsonHelper::GetString(blockData, "type");
		if (typeStr == "Movable") m_type = BlockType::Movable;
		else if (typeStr == "Wall") m_type = BlockType::Wall;
		else if (typeStr == "Goal") m_type = BlockType::Goal;
		else if (typeStr == "Moving") m_type = BlockType::Moving;
		else if (typeStr == "Transfer") m_type = BlockType::Transfer;
		else if (typeStr == "Jump") m_type = BlockType::Jump;
		else if (typeStr == "Slippery")m_type = BlockType::Slippery;
		else if (typeStr == "Rotating")m_type = BlockType::Rotating;
		else if (typeStr == "Checkpoint")m_type = BlockType::Checkpoint;
		else if (typeStr == "TutorialTrigger")m_type = BlockType::TutorialTrigger;
		else m_type = BlockType::Movable;
	}
}

nlohmann::json BlockDataComponent::ToJson() const
{
	nlohmann::json j;

	std::string typeStr = "";

	if (m_type == BlockType::Movable)
	{
		typeStr = "Movable";
	}
	else if (m_type == BlockType::Wall)
	{
		typeStr = "Wall";
	}
	else if (m_type == BlockType::Goal)
	{
		typeStr = "Goal";
	}
	else if (m_type == BlockType::Moving)
	{
		typeStr = "Moving";
	}
	else if (m_type == BlockType::Transfer)
	{
		typeStr = "Transer";
	}
	else if (m_type == BlockType::Jump)
	{
		typeStr = "Jump";
	}
	else if (m_type == BlockType::Slippery)
	{
		typeStr = "Slippery";
	}
	else if (m_type == BlockType::Rotating)
	{
		typeStr = "Rotating";
	}
	else if (m_type == BlockType::Checkpoint)
	{
		typeStr = "Checkpoint";
	}
	else if (m_type == BlockType::TutorialTrigger)
	{
		typeStr = "TutorialTrigger";
	}

	if (!typeStr.empty()) {
		j["type"] = typeStr;
	}

	return j;
}
