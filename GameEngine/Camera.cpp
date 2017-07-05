#include "Camera.h"

#include "DeviceResources.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/**
*	@brief �R���X�g���N�^
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
*	@brief �f�X�g���N�^
*/
Camera::~Camera()
{
}

/**
*	@brief �X�V
*/
void Camera::Update()
{
	// �r���[�s����v�Z
	m_Viewmat = Matrix::CreateLookAt(m_Eyepos, m_Refpos, m_Upvec);
	// �v���W�F�N�V�����s����v�Z
	m_Projmat = Matrix::CreatePerspectiveFieldOfView(m_FovY, m_Aspect, m_NearClip, m_FarClip);
}

/// <summary>
/// �R�c���Q�c���W�ϊ�
/// ���[���h���W����X�N���[�����W���v�Z����
/// </summary>
/// <param name="pos2d"></param>
/// <returns>����</returns>
bool Camera::Project(const Vector3& worldPos, Vector2* screenPos)
{
	Vector4 clipPos;
	Vector4 worldPosV4(worldPos.x, worldPos.y, worldPos.z, 1.0f);

	// �r���[�ϊ�
	clipPos = Vector4::Transform(worldPosV4, m_Viewmat);

	// �ˉe�ϊ�
	clipPos = Vector4::Transform(clipPos, m_Projmat);

	// �r���[�|�[�g�̎擾
	D3D11_VIEWPORT viewport = DX::DeviceResources::GetInstance()->GetScreenViewport();

	if (clipPos.w <= 1.0e-5) return false;

	float ndcX = clipPos.x / clipPos.w;
	float ndcY = -clipPos.y / clipPos.w;

	screenPos->x = (ndcX + 1.0f) * 0.5f * viewport.Width + viewport.TopLeftX;
	screenPos->y = (ndcY + 1.0f) * 0.5f * viewport.Height + viewport.TopLeftY;

	return true;
}
