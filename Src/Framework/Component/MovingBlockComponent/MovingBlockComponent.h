#pragma once
#include"../Component.h"

class TransformComponent;
class GameViewModel;

class MovingBlockComponent:public Component
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;

	void Configure(const nlohmann::json& data);
	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "MovingBlockComponent"; }

	void OnInspect()override;
	bool OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene)override;

	void SetStartPos(Math::Vector3& pos) { m_startPos = pos; }
	void SetEndPos(Math::Vector3& pos) { m_endPos = pos; }
	void SetDuration(float duration) { m_duration = duration; }
	const Math::Vector3& GetStartPos() const{ return m_startPos; }
	const Math::Vector3& GetEndPos()const { return m_endPos; }
	const float& GetDuration() const{ return m_duration; }
	Math::Vector3& GetStartPos_NonConst() { return m_startPos; }
	Math::Vector3& GetEndPos_NonConst() { return m_endPos; }
	void SetActive(bool active) { m_isActive = active; }
	bool IsActive()const { return m_isActive; }
	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }
	void RequestStateChangeCommand();

private:
	Math::Vector3 m_startPos{};
	Math::Vector3 m_endPos{};
	float m_duration = 2.0f;//2秒で片道移動
	std::shared_ptr<TransformComponent> m_transform;
	float m_progress = 0.0f;//進捗度
	bool m_isRevarse = false;//逆再生中かどうか
	bool m_isActive = true;//このコンポーネントが有効か
	std::weak_ptr<GameViewModel> m_wpViewModel;

	bool m_isStartGizmoDragging = false;
	bool m_isEndGizmoDragging = false;
};