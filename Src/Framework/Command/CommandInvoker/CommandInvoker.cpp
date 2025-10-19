#include "CommandInvoker.h"

void CommandInvoker::ExecuteCommand(std::unique_ptr<ICommand> command)
{
	if (command)
	{
		command->Execute();
		m_commandHistory.push_back(std::move(command));
	}
}

void CommandInvoker::UndoLastCommand()
{
	if (!m_commandHistory.empty())
	{
		m_commandHistory.back()->Undo();
		m_commandHistory.pop_back();
	}
}

void CommandInvoker::Clear()
{
	m_commandHistory.clear();
}
