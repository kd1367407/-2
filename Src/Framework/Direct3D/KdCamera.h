#pragma once

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 描画に使用する視野のデータをアプリケーション上で管理するクラス
// 3D描画に使用する各種行列を保持
// 被写界深度（目の焦点をどの距離に合わせるのか：焦点が合っていない距離はぼやける）に必要な情報を保持
// シェーダーへの情報の送信を行う（重要機能）SetToShader関数をもつ
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
class KdCamera
{
public:
	KdCamera();

	~KdCamera() {}

	// ※重要機能：カメラ情報(ビュー・射影行列など)をシェーダへ転送
	void SetToShader() const;

	// カメラ行列の設定・カメラ行列からビュー行列を生成
	virtual void SetCameraMatrix(const DirectX::SimpleMath::Matrix& mCam) { m_mCam = mCam; m_mView = m_mCam.Invert(); }

	
	Math::Vector3 GetCameraPosition()const { return m_mCam.Translation(); }
	//オイラー角（ピッチ、ヨー、ロール）からカメラの回転を設定
	void SetRotationFromEuler(float pitch, float yaw, float roll);

	//設定したワールド座標の点の方向を向く
	void SetLookAt(const Math::Vector3& targetPos);

	//カメラの座標を取ってくる
	Math::Vector3 GetPos()
	{
		return m_mCam.Translation();
	}

	//カメラの座標セット
	void SetPos(const Math::Vector3& pos)
	{
		m_mCam.Translation(pos);
	}

	float GetYaw()const { return m_yaw; }
	float GetPitch()const { return m_pitch; }

	// 射影行列の設定
	void SetProjectionMatrix(float fov, float maxRange = 2000, float minRange = 0.01f, float aspectRatio = 0.0f);
	void SetProjectionMatrix(const DirectX::SimpleMath::Matrix& rProj);

	// 焦点距離を設定
	void SetFocus(float focusDist, float focusForeRange, float focusBackRange);

	// カメラ行列取得
	inline const DirectX::SimpleMath::Matrix& GetCameraMatrix() const { return m_mCam; }

	// カメラ逆行列取得
	inline const DirectX::SimpleMath::Matrix& GetCameraViewMatrix() const { return m_mView; }

	// 射影行列取得
	inline const DirectX::SimpleMath::Matrix& GetProjMatrix() const { return m_mProj; }

	// クライアント座標（2D）から3Dワールド座標を求める用のレイ情報を生成
	void GenerateRayInfoFromClientPos(const POINT& clientPos, Math::Vector3& rayPos, Math::Vector3& rayDir, float& rayRange);
	
	//ビューポートを指定して2Dから3Dワールド座標を求める用のレイ情報を生成
	void GenerateRayInfoFromViewportPos(const POINT& clientPos, Math::Vector3& rayPos, Math::Vector3& rayDir, float& rayRange);
	
	// ワールド座標(3D座標)をスクリーン座標(2D座標)に変換する
	void ConvertWorldToScreenDetail(const Math::Vector3& pos, Math::Vector3& result);

	//ビューポートの設定と取得
	void SetViewport(const D3D11_VIEWPORT& vp) { m_viewport = vp; }
	void SetViewport(int x, int y, int w, int h, float minDepth = 0.0f, float maxDepth = 1.0f);
	void GetViewport(D3D11_VIEWPORT& vp) const { vp = m_viewport; }

	//現在のパラメータからビュー行列を強制的に再計算
	void UpdateViewMatrix();

protected:

	// カメラ行列：3Dワールド空間上のカメラの行列情報
	DirectX::SimpleMath::Matrix	m_mCam;

	// カメラ行列：逆行列情報
	DirectX::SimpleMath::Matrix	m_mView;

	// 射影行列：視野角や見える距離など
	DirectX::SimpleMath::Matrix	m_mProj;

	// 被写界深度効果に必要な情報
	float m_focusDistance = 0.0f;
	float m_focusForeRange = 0.0f;
	float m_focusBackRange = 2000.0f;

	//Y軸
	float m_yaw = 0.0f;
	//X軸
	float m_pitch = 0.0f;

	//このカメラが使用するビューポート
	D3D11_VIEWPORT m_viewport = {};
};
