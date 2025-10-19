#include "GameManager.h"

void GameManager::UpdateAfterNewStageSave(const std::string& newFilePath)
{
	m_currentStagePath = newFilePath;
	m_loadMode = LoadMode::Edit;
}
