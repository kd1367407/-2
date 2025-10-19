#pragma once
#include"../Component.h"

class GameViewModel;

//player固有のデータを管理し、かつUIでの見せ方も知っているコンセプト
class PlayerStatsComponent:public Component
{
public:
	//IInspectableとして必須の実装
	void OnInspect()override;

	int GetHp()const { return m_hp; }
	void SetHp(int hp) { m_hp = hp; }

	void Configure(const nlohmann::json& data);
	nlohmann::json ToJson() const;

	void SetInitialPos(const Math::Vector3& pos) { m_initialPos = pos; }
	const Math::Vector3& GetInitialPos() { return m_initialPos; }
	const char* GetComponentName()const override { return "PlayerStatsComponent"; }

private:
	int m_hp = 100;
	int m_maxHp = 100;
	Math::Vector3 m_initialPos = { 0,5,0 };
	bool m_isDirty = true;
};