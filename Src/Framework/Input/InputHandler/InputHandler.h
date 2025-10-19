#pragma once

class GameViewModel;
class GameObject;

//入力を専門に扱うクラス
class InputHandler
{
public:

	void Update();

	void SetViewModel(std::shared_ptr<GameViewModel>& viewModel)
	{
		m_wpViewModel = viewModel;
	}

private:
	std::weak_ptr<GameViewModel> m_wpViewModel;

	//前フレームでホバーしていたオブジェクトを保持
	std::weak_ptr<GameObject> m_wpHoverdObj;
};