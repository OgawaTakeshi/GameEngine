#include "Camera.h"

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