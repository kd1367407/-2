#include "KdCamera.h"


KdCamera::KdCamera()
{
	//ビューポートを設定
	if (KdDirect3D::Instance().GetBackBuffer())
	{
		SetViewport(0, 0, KdDirect3D::Instance().GetBackBuffer()->GetWidth(), KdDirect3D::Instance().GetBackBuffer()->GetHeight());
	}

	//射影行列のデフォルト設定
	SetProjectionMatrix(60.0f);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// カメラ情報(ビュー・射影行列など)をシェーダへ転送
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// CPU側に用意してあるシェーダ情報格納用コンテナへデータをコピー
// コンテナをシェーダー(GPU)に送信する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCamera::SetToShader() const
{
	// カメラの情報をGPUへ転送
	KdShaderManager::Instance().WriteCBCamera(m_mCam, m_mProj);

	// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
	// 被写界深度（DepthOfField）加工に使うの情報も更新
	float nearClippingDistance = -(m_mProj._43 / m_mProj._33);
	float farClippingDistance = - (m_mProj._43 / (m_mProj._33 - 1));
	float viewRange = farClippingDistance - nearClippingDistance;

	// フォーカスを合わせる焦点距離をコピー
	KdShaderManager::Instance().m_postProcessShader.SetNearClippingDistance(nearClippingDistance);
	KdShaderManager::Instance().m_postProcessShader.SetFarClippingDistance(farClippingDistance);
	KdShaderManager::Instance().m_postProcessShader.SetFocusDistance((m_focusDistance - nearClippingDistance) / viewRange);
	KdShaderManager::Instance().m_postProcessShader.SetFocusRange(m_focusForeRange / viewRange, m_focusBackRange / viewRange);
}

void KdCamera::SetRotationFromEuler(float pitch, float yaw, float roll)
{
	m_pitch = pitch;
	m_yaw = yaw;

	//上下の視点移動の制限
	m_pitch = std::max(-DirectX::XMConvertToRadians(89.0f), std::min(DirectX::XMConvertToRadians(89.0f), m_pitch));

	//オイラー角からクォータニオン(オイラー角の進化版)生成
	Math::Quaternion q = Math::Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, roll);

	//カメラの座標入れる
	Math::Vector3 currentPos = m_mCam.Translation();

	//カメラ行列更新
	m_mCam = Math::Matrix::CreateFromQuaternion(q) * Math::Matrix::CreateTranslation(currentPos);
	
	//ビュー行列更新
	m_mView = m_mCam.Invert();
}

void KdCamera::SetLookAt(const Math::Vector3& targetPos)
{
	//カメラの現在位置からtargetposへの方向計算
	Math::Vector3 dir = targetPos - m_mCam.Translation();
	dir.Normalize();

	//dirから上下首振り計算
	float horizontalDistance = sqrt(dir.x * dir.x + dir.z * dir.z);
	m_pitch = atan2(-dir.y, horizontalDistance);

	//dirから横振り計算
	m_yaw = atan2(dir.x, dir.z);

	UpdateViewMatrix();
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 射影行列の設定：各種パラメータから射影行列を生成して保持する
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// 引数：（ fov：視野角 ）（ maxRange：描画する最長距離 ）（ minRange：描画する最短距離 ）（ aspectRatio：画面の縦横幅の比率 ）
// 視野角以外のパラメータはデフォルト引数が設定されているため、省略可能
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCamera::SetProjectionMatrix(float fov, float maxRange, float minRange, float aspectRatio)
{
	float aspect = aspectRatio;

	// アスペクト比が不正だった場合
	if (aspect <= 0)
	{
		// 自動的にバックバッファからアスペクト比を求める
		if (KdDirect3D::Instance().GetBackBuffer())
		{
			aspect = KdDirect3D::Instance().GetBackBuffer()->GetAspectRatio();
		}
		// バックバッファが生成されてすらいな状況なら射影行列をセットしない
		else
		{
			return;
		}
	}

	m_mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fov), aspect, minRange, maxRange);

	SetProjectionMatrix(m_mProj);
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 射影行列の設定：既存の射影行列をコピーする
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCamera::SetProjectionMatrix(const DirectX::SimpleMath::Matrix& rProj)
{
	m_mProj = rProj;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// 被写界深度の情報を設定
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// カメラから見て焦点を当てる距離の設定
// ぼかさずに描画する焦点エリアを前後それぞれ別の距離に設定可能
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCamera::SetFocus(float focusDist, float focusForeRange, float focusBackRange)
{
	m_focusDistance = focusDist;
	m_focusForeRange = focusForeRange;
	m_focusBackRange = focusBackRange;
}

// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
// クライアント座標（2D）から3Dワールド座標を求める用のレイ情報を生成
// ----- ----- ----- ----- ----- ----- ----- ----- ----- -----
// マウスポインタの2D位置にある3Dオブジェクトを選択する時などに使用する
// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////
void KdCamera::GenerateRayInfoFromClientPos(const POINT& clientPos, Math::Vector3& rayPos, Math::Vector3& rayDir, float& rayRange)
{
	// レイ判定の最遠座標
	Math::Vector3 farPos;

	// 2D座標を3Dワールド座標に変換する関数を使ってレイの発射座標と最遠座標を求める
	KdDirect3D::Instance().ClientToWorld(clientPos, 0.0f, rayPos, m_mCam, m_mProj);
	KdDirect3D::Instance().ClientToWorld(clientPos, 1.0f, farPos, m_mCam, m_mProj);

	// 目的地 - 出発地点 でレイを飛ばす方向を求める
	rayDir = farPos - rayPos;

	// 2点間の距離を求めてレイの判定距離を求める
	rayRange = rayDir.Length();

	// 正規化（長さを1.0fに）して方向ベクトルに変換する
	rayDir.Normalize();
}

void KdCamera::GenerateRayInfoFromViewportPos(const POINT& clientPos, Math::Vector3& rayPos, Math::Vector3& rayDir, float& rayRange)
{
	//各行列の逆行列の算出
	Math::Matrix invView = GetCameraMatrix();
	Math::Matrix invProj = GetProjMatrix().Invert();

	//ビューポート行列の逆行列を計算(スクリーン座標->NDC座標への変換)
	Math::Matrix invViewport;
	invViewport._11 = 2.0f / m_viewport.Width;
	invViewport._22 = -2.0f / m_viewport.Height;
	invViewport._41 = -1.0f - (m_viewport.TopLeftX * invViewport._11);
	invViewport._42 = 1.0f - (m_viewport.TopLeftY * invViewport._22);

	//2D->3D変換用の行列を合成
	Math::Matrix convertMat = invViewport * invProj * invView;

	//変換行列を使って近平面と遠平面上の3D座標を計算
	Math::Vector3 nearPos, farPos;
	nearPos = Math::Vector3::Transform({ (float)clientPos.x, (float)clientPos.y, 0.0f }, convertMat);
	farPos = Math::Vector3::Transform({ (float)clientPos.x, (float)clientPos.y, 1.0f }, convertMat);

	//レイ情報を設定
	rayPos = nearPos;

	// 目的地 - 出発地点 でレイを飛ばす方向を求める
	rayDir = farPos - rayPos;

	// 2点間の距離を求めてレイの判定距離を求める
	rayRange = rayDir.Length();

	// 正規化（長さを1.0fに）して方向ベクトルに変換する
	rayDir.Normalize();
}

// ワールド座標(3D座標)をスクリーン座標(2D座標)に変換する
void KdCamera::ConvertWorldToScreenDetail(const Math::Vector3& pos, Math::Vector3& result)
{
	// ビューポートを取得する
	Math::Viewport vp;
	KdDirect3D::Instance().CopyViewportInfo(vp);

	// ①ワールド変換行列×ビュー行列×射影行列
	Math::Matrix world = Math::Matrix::CreateTranslation(pos);
	Math::Matrix wvp = world * GetCameraViewMatrix() * GetProjMatrix();

	// ②奥行情報(w = ._44で割る必要がある)
	wvp._41 /= wvp._44;
	wvp._42 /= wvp._44;
	wvp._43 /= wvp._44;

	// 射影行列系での2D(みたいな)座標
	// ↑-1~1の範囲の座標の事
	Math::Vector3 localPos = wvp.Translation();

	// ここで幅や高さを考慮して計算する(これで正確なスクリーン座標になる)
	result.x = localPos.x * (vp.width * 0.5f);
	result.y = localPos.y * (vp.height * 0.5f);
	result.z = wvp._44;
}

void KdCamera::SetViewport(int x, int y, int w, int h, float minDepth, float maxDepth)
{
	m_viewport.TopLeftX = static_cast<FLOAT>(x);
	m_viewport.TopLeftY = static_cast<FLOAT>(y);
	m_viewport.Width = static_cast<FLOAT>(w);
	m_viewport.Height = static_cast<FLOAT>(h);
	m_viewport.MinDepth = minDepth;
	m_viewport.MaxDepth = maxDepth;
}

void KdCamera::UpdateViewMatrix()
{
	// 現在の位置と回転（ヨー、ピッチ）からクォータニオンを生成
	Math::Quaternion q = Math::Quaternion::CreateFromYawPitchRoll(m_yaw, m_pitch, 0.0f);

	// 現在の位置を取得
	Math::Vector3 currentPos = m_mCam.Translation();

	// クォータニオンと位置から、最新のカメラ行列を再計算
	m_mCam = Math::Matrix::CreateFromQuaternion(q) * Math::Matrix::CreateTranslation(currentPos);

	// 最新のカメラ行列から、最新のビュー行列を計算
	m_mView = m_mCam.Invert();
}
