#pragma once
#include"../Component.h"
#include"../Src/Application/GameData/GameType/GameType.h"

//ブロックの種類というデータを管理するコンポーネント
class BlockDataComponent :public Component
{
public:
	BlockType GetType()const { return m_type; }
	void SetType(BlockType type) { m_type = type; }

	void Configure(const nlohmann::json& data);
	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "BlockDataComponent"; }

private:
	BlockType m_type = BlockType::None;
};