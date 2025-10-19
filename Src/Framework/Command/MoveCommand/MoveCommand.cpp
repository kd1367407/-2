#include "MoveCommand.h"
#include"../../Component/RigidbodyComponent/RigidbodyComponent.h"

MoveCommand::MoveCommand(std::shared_ptr<RigidbodyComponent> target, const Math::Vector3& force):
	m_wpTarget(target), m_force(force)
{
}

void MoveCommand::Execute()
{
	if (auto target = m_wpTarget.lock())
	{
		target->AddForce(m_force);
	}
}

void MoveCommand::Undo()
{
	//特に必要ない
}
