#pragma once
#include"../Component.h"
#include"../ICollisionReceiver.h"

class GameObject;
class GameViewModel;

class TransferBlockComponent :public Component, public ICollisionReceiver
{
public:
	void Awake()override;
	void Start()override;
	void Update()override;
	void OnCollision(const CollisionInfo& info)override;

	void SetPartner(const std::shared_ptr<GameObject>& partner) { m_wpPartner = partner; }
	std::shared_ptr<GameObject> GetPartner() { return m_wpPartner.lock(); }
	int GetTransferID()const { return m_transfarID; }
	void SetTransferID(int id) { m_transfarID = id; }
	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }

	void OnInspect()override;

	virtual void Configure(const nlohmann::json& data);

	nlohmann::json ToJson() const override;

	const char* GetComponentName()const override { return "TransferBlockComponent"; }

private:
	int m_transfarID = 0;
	std::weak_ptr<GameObject> m_wpPartner;
	std::weak_ptr<GameViewModel> m_wpViewModel;
	float m_coolDown = 0.0f;
};