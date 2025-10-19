#pragma once

struct CollisionInfo;

class ITriggerReceiver
{
public:
	virtual ~ITriggerReceiver() = default;
	virtual void OnTriggerEnter(const CollisionInfo& info) = 0;

private:

};