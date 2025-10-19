#pragma once
#include"../Component.h"

class TagComponent:public Component
{
public:
	const char* GetComponentName() const override { return "TagComponent"; }

private:

};