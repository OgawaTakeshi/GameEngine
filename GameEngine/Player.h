/**
*	@file	Player.h
*
*	@brief	���@
*
*	@date	2015/05/08
*
*	@author	���V�@�L��
*/
#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>
#include "Obj3D.h"
#include "CollisionNode.h"
#include "LandShape.h"

// ���@
class Player
{
public:
	// ���{�b�g�p�[�c
	enum
	{
		PARTS_TANK,		// �L���^�s��
		PARTS_WAIST,	// ��
		PARTS_BREAST,	// ��
		PARTS_HEAD,		// ��
		PARTS_ARM_R,	// �E�r
		PARTS_GUN_R,	// �E�̏e

		PARTS_NUM
	};

	// �W�����v����
	static const float JUMP_SPEED_FIRST;
	static const float JUMP_SPEED_MAX;
	// �d�͉���
	static const float GRAVITY_ACC;

	// �R���X�g���N�^
	Player();
	// �f�X�g���N�^
	~Player();
	// ������
	void Initialize();
	void ControlNormal();
	void ControlLockOn();
	// ���t���[���X�V
	void Update();
	// �s��̌v�Z
	void Calc();
	// �`��
	void Draw();
	// ���W���擾
	const DirectX::SimpleMath::Vector3& GetTrans();
	// ���W��ݒ�
	void SetTrans(const DirectX::SimpleMath::Vector3& trans);
	// ��]��ݒ�
	void SetRot(const DirectX::SimpleMath::Vector3& rot);
	// ���[���h�s����擾
	const DirectX::SimpleMath::Matrix& GetLocalWorld();
	void Load();
	// �{�̂̒n�`�����蔻�苅���擾
	const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }
	// �e�ۂ̓����蔻��J�v�Z�����擾
	const SphereNode& GetCollisionNodeBullet() { return m_CollisionNodeBullet; }

	// ����
	void FireBullet();
	// �߂�
	void ResetBullet();

	// ���W���擾
	const DirectX::SimpleMath::Vector3& GetVelocity() { return m_Velocity; }

	void StartJump();
	void StartFall();
	void StopJump();

	void SetLandShapeArray(std::vector<std::unique_ptr<LandShape>>*	pLandShapeArray) { m_pLandShapeArray = pLandShapeArray; }

protected:
	// �����o�ϐ�
	// ���{�b�g�̍\���p�[�c
	std::vector<Obj3D>	m_Obj;

	// �e
	Obj3D m_ObjShadow;
	// �M�~�b�N�̉ғ��T�C�N��
	float m_cycle;

	bool m_FireFlag;
	int m_FireCount;
	DirectX::SimpleMath::Vector3 m_BulletVel;

	// �{�̂̒n�`�����蔻�苅
	SphereNode m_CollisionNodeBody;

	// �e�ۂ̓����蔻�苅
	SphereNode m_CollisionNodeBullet;

	// ���x
	DirectX::SimpleMath::Vector3 m_Velocity;

	// �W�����v�i�����j�����H
	bool m_isJump;

	std::vector<std::unique_ptr<LandShape>>*	m_pLandShapeArray;
};

