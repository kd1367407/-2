#pragma once
#include"../Component.h"
#include"../Src/Application/GameViewModel.h"
#include"../Src/Application/main.h"

class TransformComponent:public Component
{
public:
	TransformComponent() { m_updatePriority = -100; }//何も依存していないので一番最初に初期化させる

	void PostUpdate()override;

	//IInspectableとして必須の実装
	void OnInspect()override;
	
	//移動
	void Move(const Math::Vector3& delta)
	{
		m_pos += delta;
		m_isDirty = true;
	}

	//変更コマンドの発行をViewModelに依頼
	void RequestTransformChangeCommand();

	void Configure(const nlohmann::json& data);

	nlohmann::json ToJson() const override;
	const char* GetComponentName()const override { return "TransformComponent"; }
	bool OnDrawGizmos(const EditorGizmoContext& context, GameScene& scene)override;

	
	//--セッター--
	void SetPos(const Math::Vector3& pos) 
	{
		m_pos = pos; 
		m_isDirty = true;
	}

	void SetRot(const Math::Vector3& rot)
	{
		m_rot = rot;
		m_isDirty = true;
	}

	void SetScale(const Math::Vector3& scale)
	{
		m_scale = scale;
		m_isDirty = true;
	}

	void SetMatrix(const Math::Matrix& mat) { m_worldMat = mat; }

	void SetViewModel(const std::shared_ptr<GameViewModel>& viewModel) { m_wpViewModel = viewModel; }

	//--ゲッター--
	const Math::Matrix& GetMatrix()const { return m_worldMat; }

	const Math::Vector3& GetPos()const { return m_pos; }

	const Math::Vector3& GetRot()const { return m_rot; }

	const Math::Vector3& GetScale()const { return m_scale; }

	//--非constバージョン(ImGuiでの編集のため)--
	Math::Vector3& GetPos_NonConst() { m_isDirty = true; return m_pos; }

	Math::Vector3& GetRot_NonConst() { m_isDirty = true; return m_rot; }

	Math::Vector3& GetScale_NonConst() { m_isDirty = true; return m_scale; }

private:
	Math::Vector3 m_pos = Math::Vector3::Zero;
	Math::Vector3 m_rot = Math::Vector3::Zero;
	Math::Vector3 m_scale = Math::Vector3::One;
	Math::Matrix m_worldMat = Math::Matrix::Identity;
	bool m_isDirty = true;
	std::weak_ptr<GameViewModel> m_wpViewModel;
	bool m_isGizmoDragging = false;
};