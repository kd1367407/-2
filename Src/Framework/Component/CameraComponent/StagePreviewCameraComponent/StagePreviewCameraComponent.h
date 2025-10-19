#pragma once
#include"../CameraComponent.h"

class StagePreviewCameraComponent:public CameraComponent
{
public:
	void AdjustCameraToShowAll(const std::list<std::shared_ptr<GameObject>>& objects);

private:

};