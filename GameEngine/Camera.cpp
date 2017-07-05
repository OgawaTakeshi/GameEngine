#include "Camera.h"

#include "DeviceResources.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/**
*	@brief コンストラクタ
*/
Camera::Camera()
{
	m_Viewmat = Matrix::Identity;
	m_Eyepos = Vector3(0.0f, 6.0f, 10.0f);
	m_Refpos = Vector3(0.0f, 2.0f, 0.0f);
	m_Upvec = Vector3(0.0f, 1.0f, 0.0f);

	m_Projmat = Matrix::Identity;
	m_FovY = XMConvertToRadians(60.0f);
	m_Aspect = (640.0f / 480.0f);
	m_NearClip = 0.1f;
	m_FarClip = 1000.0f;
}

/**
*	@brief デストラクタ
*/
Camera::~Camera()
{
}

/**
*	@brief 更新
*/
void Camera::Update()
{
	// ビュー行列を計算
	m_Viewmat = Matrix::CreateLookAt(m_Eyepos, m_Refpos, m_Upvec);
	// プロジェクション行列を計算
	m_Projmat = Matrix::CreatePerspectiveFieldOfView(m_FovY, m_Aspect, m_NearClip, m_FarClip);
}

/// <summary>
/// ３Ｄ→２Ｄ座標変換
/// ワールド座標からスクリーン座標を計算する
/// </summary>
/// <param name="pos2d"></param>
/// <returns>成否</returns>
bool Camera::Project(const Vector3& worldPos, Vector2* screenPos)
{
	Vector4 clipPos;
	Vector4 worldPosV4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

	// ビュー変換
	clipPos = Vector4::Transform(worldPosV4, m_Viewmat);

	// 射影変換
	clipPos = Vector4::Transform(clipPos, m_Projmat);

	// ビューポートの取得
	D3D11_VIEWPORT viewport = DX::DeviceResources::GetInstance()->GetScreenViewport();

	if (clipPos.w <= 1.0e-5) return false;

	float ndcX = clipPos.x / clipPos.w;
	float ndcY = -clipPos.y / clipPos.w;

	screenPos->x = (ndcX + 1.0f) * 0.5f * viewport.Width + viewport.TopLeftX;
	screenPos->y = (ndcY + 1.0f) * 0.5f * viewport.Height + viewport.TopLeftY;

	return true;
}
