#pragma once

// ゲーム上に存在するすべてのオブジェクトの基底となるクラス
class KdGameObject : public std::enable_shared_from_this<KdGameObject>
{
public:

	// どのような描画を行うのかを設定するTypeID：Bitフラグで複数指定可能
	enum
	{
		eDrawTypeLit = 1 << 0,
		eDrawTypeUnLit = 1 << 1,
		eDrawTypeBright = 1 << 2,
		eDrawTypeUI = 1 << 3,
		eDrawTypeDepthOfShadow = 1 << 4,
	};

	KdGameObject() {}
	virtual ~KdGameObject() { Release(); }

	//自分の型名を返す
	virtual const char* GetTypeName() const = 0;

	//オブジェクトの名前を取得
	const std::string& GetName()const { return m_name; }
	//オブジェクトの名前を設定
	void SetName(const std::string& name)
	{
		m_name = name;
	}

	// 生成される全てに共通するパラメータに対する初期化のみ
	virtual void Init() {}

	//更新
	virtual void PreUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate();

	//描画
	virtual void GenerateDepthMapFromLight() {}
	virtual void PreDraw() {}
	virtual void DrawLit() {}
	virtual void DrawUnLit() {}
	virtual void DrawBright() {}
	virtual void DrawSprite() {}
	virtual void PostDraw(){}
	virtual void DrawDebug();

	//各種機能
	virtual void SetAsset(const std::string&) {}

	const Math::Vector3& GetPos()const { return m_pos; }
	void SetPos(const Math::Vector3& pos) { m_pos = pos; m_isDirty = true; }

	const Math::Vector3& GetRot()const { return m_rot; }
	void SetRot(const Math::Vector3& rot) { m_rot = rot; m_isDirty = true; }

	const Math::Vector3& GetScale()const { return m_scale; }
	void SetScale(const Math::Vector3& scale) { m_scale = scale; m_isDirty = true; }
	void SetScale(float scalar) { SetScale({ scalar,scalar,scalar }); }

	const Math::Matrix& GetMatrix()const { return m_mWorld; }

	virtual bool IsExpired() const { return m_isExpired; }

	//オブジェクトを削除リストへ登録
	void Expire() { m_isExpired = true; }

	//当たり判定
	bool Intersects(const KdCollider::SphereInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::BoxInfo& targetBox, std::list<KdCollider::CollisionResult>* pResults);
	bool Intersects(const KdCollider::RayInfo& targetShape, std::list<KdCollider::CollisionResult>* pResults);

	//ImGuiのUIを描画するための仮想関数
	virtual void OnImGui()
	{
		ImGui::Text("Name: %s", m_name.c_str());
		ImGui::Text("Type: %s", GetTypeName());

		//全オブジェクト共通の座標編集UIを提供
		//Math::Vector3 pos = GetPos();
		if (ImGui::DragFloat3("Pos", &m_pos.x, 0.1f))
		{
			//SetPos(m_pos);
			m_isDirty = true;
		}
		if (ImGui::DragFloat3("Rot", &m_rot.x, 1.0f))
		{
			m_isDirty = true;
		}
		if (ImGui::DragFloat3("Scale", &m_scale.x, 0.01f))
		{
			m_isDirty = true;
		}
	}

	virtual bool IsVisible()	const { return false; }
	virtual bool IsRideable()	const { return false; }

	// 視錐台範囲内に入っているかどうか
	virtual bool CheckInScreen(const DirectX::BoundingFrustum&) const { return false; }

	// カメラからの距離を計算
	virtual void CalcDistSqrFromCamera(const Math::Vector3& camPos);

	float GetDistSqrFromCamera() const { return m_distSqrFromCamera; }

	UINT GetDrawType() const { return m_drawType; }

protected:

	void Release() {}

	// 描画タイプ・何の描画を行うのかを決める / 最適な描画リスト作成用
	UINT m_drawType = 0;

	// カメラからの距離
	float m_distSqrFromCamera = 0;

	// 存在消滅フラグ
	bool m_isExpired = false;

	// 当たり判定クラス
	std::unique_ptr<KdCollider> m_pCollider = nullptr;

	// デバッグ情報クラス
	std::unique_ptr<KdDebugWireFrame> m_pDebugWire = nullptr;

	//オブジェクトの名前
	std::string m_name = "GameObject";

	//位置の設計図
	Math::Vector3 m_pos = Math::Vector3::Zero;

	//回転の設計図
	Math::Vector3 m_rot = Math::Vector3::Zero;

	//拡縮の設計図
	Math::Vector3 m_scale = Math::Vector3::One;

	// 3D空間に存在する機能
	Math::Matrix	m_mWorld = Math::Matrix::Identity;

	//行列の再計算をするかどうか
	bool m_isDirty = true;
};
