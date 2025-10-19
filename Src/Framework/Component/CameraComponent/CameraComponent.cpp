#include "CameraComponent.h"
#include"../GameObject.h"
#include"../TransformComponent/TransformComponent.h"
#include"../Src/Application/Scene/SceneManager.h"
#include"../Src/Application/System/PhysicsSystem.h"
#include"../Src/Application/main.h"
#include"../Src/Application/SettingsManager/SettingsManager.h"
#include"../Src/Application/JsonHelper/JsonHelper.h"

void CameraComponent::Awake()
{
	//カメラインスタンス生成
	m_spCamera = std::make_shared<KdCamera>();
	m_spCamera->SetProjectionMatrix(60.0f);
}

void CameraComponent::Start()
{
	//自身がアタッチされているGameObjectから必要なコンポーネントを取得
	m_transform = m_owner->GetComponent<TransformComponent>();
}