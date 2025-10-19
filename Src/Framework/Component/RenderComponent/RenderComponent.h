#pragma once
#include"../Component.h"

class TransformComponent;
class GameViewModel;

//3Dモデルの描画を担当するコンポーネント
class RenderComponent :public Component
{
public:
	//ハイライトの状態
	enum class HighlightState
	{
		None,
		Hoverd,//カーソルが乗っている
		Selected//選択されている
	};


	void Awake()override;
	void Start()override;

	void Configure(const nlohmann::json& data);
	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "RenderComponent"; }

	void DrawLit()override;
	void DrawBright()override;

	void SetModel(const std::shared_ptr<KdModelData>& model);
	void SetModel(const std::string& path);

	std::string GetModelPath() const { return m_modelPath; }

	//ハイライトの状態を外部が設定、取得
	void SetHighlightState(HighlightState state) { m_highlightState = state; }
	HighlightState GetHighlightState()const { return m_highlightState; }

	void OnInspect()override;

	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }

	void SetEnable(bool bEnable) { m_enable = bEnable; }
	bool IsEnable() const { return m_enable; }

private:
	std::shared_ptr<KdModelData> m_spModel;
	std::string m_modelPath;
	std::weak_ptr<GameViewModel> m_wpViewModel;

	//描画に必要な他のコンポーネントへの参照を保持する
	std::shared_ptr<TransformComponent> m_transform;

	//ハイライトの状態管理
	HighlightState m_highlightState = HighlightState::None;

	//描画するかどうか
	bool m_enable = true;
};
