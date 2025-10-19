#pragma once

class IStageObserver
{
public:
	virtual ~IStageObserver() = default;

	//モデルから状態が変化したという通知を受け取る
	virtual void OnStageStateChanged(UINT updateObjectId = UINT_MAX) = 0;
};