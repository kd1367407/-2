#pragma once
#include"../ImGuizmo/ImGuizmo.h"

struct EditorGizmoContext
{
	const Math::Matrix* viewMat = nullptr;
	const Math::Matrix* projMat = nullptr;
	ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;

	float rectX = 0;
	float rectY = 0;
	float rectW = 0;
	float rectH = 0;
};