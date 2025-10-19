#pragma once
#include"../Component.h"

//汎用的なインスペクタ(検査官)として機能する唯一のコンポーネント
class ImGuiComponent:public Component
{
public:
	//このコンポーネントがアタッチされたGameObjectのインスペクタUIを描画する
	void DrawImGui();

	const char* GetComponentName()const override { return "ImGuiComponent"; }

private:
	
};