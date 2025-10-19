#pragma once
#include"../Component.h"

//GameObjectに一意なIDを持たせるためのコンポーネント
class IdComponent:public Component
{
public:
	unsigned int GetId()const { return m_id; }

	void SetID(UINT id) { m_id = id; }

	const char* GetComponentName()const override { return "IdComponent"; }

private:
	friend class GameViewModel;//ViewModelだけがIDを設定出来るようにするため
	unsigned int m_id = 0;
};