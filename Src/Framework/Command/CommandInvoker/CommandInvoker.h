#pragma once
#include"../Command.h"

class CommandInvoker
{
public:
	void ExecuteCommand(std::unique_ptr<ICommand> command);
	void UndoLastCommand();
	void Clear();

private:
	std::vector<std::unique_ptr<ICommand>> m_commandHistory;
};