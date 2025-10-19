#pragma once
#include"../Command.h"

class RigidbodyComponent;

class MoveCommand:public ICommand
{
public:
	MoveCommand(std::shared_ptr<RigidbodyComponent> target, const Math::Vector3& force);
	void Execute()override;
	void Undo()override;

private:
	std::weak_ptr<RigidbodyComponent> m_wpTarget;
	Math::Vector3 m_force;
};