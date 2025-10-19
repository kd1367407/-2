#pragma once
#include"../ImGuizmo/ImGuizmo.h"
#include"../Src/Application/GameData/BlockState/BlockState.h"
#include"../Component/GameObject.h"
#include"../Component/SolutionVisualizerComponent/SolutionVisualizerTypes/SolutionVisualizerTypes.h"

class GameScene;

class Editor
{
public:
	Editor() = default;
	~Editor() = default;

	void Init();
	//メインの描画
	void Draw(GameScene& scene);

	//レンダーターゲット取得
	KdRenderTargetPack& GetSceneRT() { return m_sceneRT; }

private:
	//グループの定義
	struct ObjectGroup
	{
		std::string name;
		std::vector<UINT> memberObjectIDs;
	};

	//交換対象
	struct SwapTarget
	{
		enum class Type
		{
			None,
			Object,
			Group
		};
		Type type = Type::None;
		std::weak_ptr<GameObject> object;
		ObjectGroup* group = nullptr;

		std::string GetName() const
		{
			if (type == Type::Object && !object.expired())
			{
				return object.lock()->GetName();
			}
			if (type == Type::Group && group)
			{
				return group->name;
			}
			return "None";
		}
	};

	//複数選択のときのインスペクターモード
	enum class MultiEditMode
	{
		Gizmo,
		Batch
	};

	//各ウィンドウの描画
	void DrawHierarchyWindow(GameScene& scene);
	void DrawInspectorWindow(GameScene& scene);
	void DrawSceneViewWindow(GameScene& scene);
	void DrawSwapToolWindow(GameScene& scene);
	void DrawMultiEditTransform(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj);
	void DrawMultiEditMovingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj);
	void DrawMultiEditSinkingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj);
	void DrawMultiEditScalingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj);
	void DrawMultiEditRotatingBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj);
	void DrawMultiEditSlipperyBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj);
	void DrawMultiEditJumpBlock(GameScene& scene, std::vector<std::weak_ptr<GameObject>>& selectedObj);

	void DrawMultiGizmoTransform();

	Math::Vector3 CalculateTargetPivot(const Editor::SwapTarget& target, const GameScene& scene);

	//現在選択されているオブジェクト
	std::vector< std::weak_ptr<GameObject>> m_selectedObjects;

	//交換スロット
	SwapTarget m_swapSlotA;
	SwapTarget m_swapSlotB;

	//グループのインデックスを保持
	int m_selectedGroupIndex = -1;

	//作成したグループのリスト
	std::vector<ObjectGroup> m_groups;

	//3Dシーンを描画するためのレンダーターゲット
	KdRenderTargetPack  m_sceneRT;

	//ImGuizmoの操作モードを管理
	ImGuizmo::OPERATION m_currentGizmoOperation = ImGuizmo::TRANSLATE;

	//座標交換ウィンドウを強制的に開く
	bool m_forceOpenSwapWindow = false;

	//コピーしたBlockStateを保持
	std::vector<BlockState> m_clipboard;

	//矢印描画フラグ
	bool m_bShowSolutionPath = false;

	//矢印描画モード
	SolutionViewMode m_solutionViewMode = SolutionViewMode::Static;

	//複数選択のインスペクターモード
	MultiEditMode m_multiEditMode = MultiEditMode::Gizmo;

	bool m_isGizmoDragging = false;
	Math::Matrix m_beforeGizmoMat;
	std::vector<Math::Matrix> m_beforeObjsMat;
	Math::Matrix m_gizmoMat;
};
