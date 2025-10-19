#pragma once
#include"../Component.h"
#include"../ITriggerReceiver.h"

class TutorialTriggerComponent:public Component,public ITriggerReceiver
{
public:
	void Awake()override;

	void OnTriggerEnter(const CollisionInfo& info)override;

	void OnInspect()override;
	nlohmann::json ToJson() const override;
	void Configure(const nlohmann::json& data) override;

	const char* GetComponentName()const override { return "TutorialTriggerComponent"; }

private:
	std::string m_tutorialBlockName;
	std::string m_tutorialText;
	std::string m_tutorialImagePath;
	bool m_hasTriggered = false;
};