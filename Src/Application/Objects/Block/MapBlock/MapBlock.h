#pragma once

class MapBlock :public KdGameObject
{
public:
	MapBlock() {}
	~MapBlock()override {}

	void Init()override;
	void Update()override;
	void DrawLit()override;

	//ImGui関係
	//オブジェクトのIDと初期座標を設定する
	void SetParameter(const std::string& id, const Math::Vector3& pos);
	const std::string& GetId()const { return m_id; }
	Math::Vector3& GetPos(){return m_pos;}

private:
	std::string m_id;
	Math::Vector3 m_pos;

	static std::shared_ptr<KdModelData> s_spModel;
};