/**
*	@file	Camera.cpp
*
*	@brief	�J����
*
*	@date	2012/04/23
*
*	@author	���V�@�L��
*/
#include "FollowCamera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/**
*	@brief �R���X�g���N�^
*/
FollowCamera::FollowCamera()
{
	m_TargetPos = Vector3::Zero;
	m_TargetAngle = 0.0f;
	m_Distance = 5.0f;
	m_Offset = Vector3(0, 2, 0);
}

/**
*	@brief �f�X�g���N�^
*/
FollowCamera::~FollowCamera()
{

}

/**
*	@brief �X�V
*/
void FollowCamera::Update()
{
	SetTargetPos(m_Target->GetTrans());
	const Matrix& targetWorld = m_Target->GetLocalWorld();

	// �Ǐ]�J����
	const float CAMERA_DISTANCE = 5.0f;
	Vector3 eyepos, refpos;

	refpos = m_TargetPos + Vector3(0, 2, 0);
	Vector3 cameraV(0, 0, CAMERA_DISTANCE);

	//Matrix rotmat = Matrix::CreateRotationY(m_TargetAngle);
	cameraV = Vector3::TransformNormal(cameraV, targetWorld);

	eyepos = refpos + cameraV;

	Vector3 eyeposPre = GetEyepos();
	Vector3 refposPre = GetRefpos();

	eyepos = eyeposPre + (eyepos - eyeposPre) * 0.05f;
	refpos = refposPre + (refpos - refposPre) * 0.20f;

	SetEyepos(eyepos);
	SetRefpos(refpos);

	// �J�����̍X�V
	Camera::Update();
}

void FollowCamera::SetTargetPos(DirectX::SimpleMath::Vector3 target)
{
	m_TargetPos = target;
}

void FollowCamera::SetTargetAngle(float target)
{
	m_TargetAngle = target;
}
