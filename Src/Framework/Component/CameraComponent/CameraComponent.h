#pragma once
#include"../Component.h"
#include"ICameraComponent/ICameraComponent.h"

class TransformComponent;

//ビュー行列の計算
class CameraComponent:public Component,public ICameraComponent
{
public:
	void Awake()override;
	void Start()override;

	//このコンポーネントが管理するKdCameraインスタンス取得
	const std::shared_ptr<KdCamera>& GetCamera() const override { return m_spCamera; }

	GameObject* GetOwner() const override { return m_owner; }

	const char* GetComponentName()const override { return "CameraComponent"; }

protected:
	std::shared_ptr<KdCamera> m_spCamera;
	std::shared_ptr<TransformComponent> m_transform;
};