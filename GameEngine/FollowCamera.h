/**
*	@file	FollowCamera.h
*
*	@brief	�J����
*
*	@date	2012/04/23
*
*	@author	���V�@�L��
*/
#pragma once

#include "Camera.h"

// �J����
class FollowCamera : public Camera
{
public:
	// �R���X�g���N�^
	FollowCamera();
	// �f�X�g���N�^
	virtual ~FollowCamera();
	// �X�V
	void Update();

	void SetTargetPos(DirectX::SimpleMath::Vector3 target);

	void SetTargetAngle(float target);
private:
	// �^�[�Q�b�g���W
	DirectX::SimpleMath::Vector3 m_TargetPos;
	// �^�[�Q�b�g�p�x
	float m_TargetAngle;
	// ����
	float m_Distance;
	// �Q�Ɠ_�I�t�Z�b�g
	DirectX::SimpleMath::Vector3 m_Offset;
};
