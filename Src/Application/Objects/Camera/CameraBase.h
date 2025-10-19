#pragma once

class CameraBase:public KdGameObject
{
public:
	CameraBase(){}
	~CameraBase()override{}

	void Init()override;
	void PreDraw()override;

	const char* GetTypeName() const override { return "CameraBase"; }

	void OnImGui()override;

	//追跡ターゲット設定
	void SetTarget(const std::shared_ptr<KdGameObject>& target);

	//KdCameraのインスタンスをかえす
	const std::shared_ptr<KdCamera>& GetCamera()const { return m_spCamera; }

	//回転行列取得
	const Math::Matrix GetRotationMatrix()const;
	//回転行列取得(Y軸)
	const Math::Matrix GetRotationYMatrix()const;

	//TPSカメラのめり込み防止の為の当たり判定対象のオブジェクトの登録
	void RegistHitObject(const std::shared_ptr<KdGameObject>& object);

protected:
	//マウスによる視点操作
	void UpdateRotateByMouse();

	//カメラ情報
	std::shared_ptr<KdCamera> m_spCamera = nullptr;

	//ターゲット情報
	std::weak_ptr<KdGameObject> m_wpTarget;

	//当たり判定オブジェクト情報
	std::vector< std::weak_ptr<KdGameObject>> m_wpHitObjList;

	//カメラ座標の中間行列
	Math::Matrix m_mLocalPos = Math::Matrix::Identity;

	//カメラ回転の中間行列
	Math::Matrix m_mRot = Math::Matrix::Identity;

	//マウス操作用
	Math::Vector3 m_degAng = Math::Vector3::Zero;
	POINT m_fixMousePos{};
};