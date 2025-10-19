#pragma once
#include"../Component.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

class SlipperyComponent:public Component
{
public:
	const char* GetComponentName() const override { return "SlipperyComponent"; }

	void Configure(const nlohmann::json& data)override
	{
		if (data.is_null() || !data.contains("SlipperyComponent"))return;

		const auto& slipperData = data.at("SlipperyComponent");

		m_dragCoefficient = JsonHelper::GetFloat(slipperData, "drag_coefficient", m_dragCoefficient);
	}

	nlohmann::json ToJson() const override
	{
		nlohmann::json j;

		j["drag_coefficient"] = m_dragCoefficient;

		return j;
	}

	void OnInspect()override;

	const float& GetDragCoefficient() const{ return m_dragCoefficient; }
	void SetDragCoefficient(float value) { m_dragCoefficient = value; }

private:
	//空気抵抗
	float m_dragCoefficient = 1.0f;
};