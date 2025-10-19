#pragma once


class EditableObject:public KdGameObject
{
public:
	EditableObject(){}
	~EditableObject()override = default;

	void Init()override;
	void DrawLit()override;

	//オブジェクトの識別に使う名前を設定
	void SetName(const std::string& name) { m_name = name; }

	//オブジェクトの名前を取得
	const std::string& GetName()const { return m_name; }

	//オブジェクトを削除リストへ登録
	void Expire() { m_isExpired = true; }

	//ImGui用
	void OnImGui()override;

private:
	//表示用モデルデータ
	std::shared_ptr <KdModelData> m_spModel = nullptr;

	//オブジェクトを識別する名前
	std::string m_name = "Unnamed";
};