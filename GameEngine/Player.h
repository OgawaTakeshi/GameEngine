/**
* @file Player.h
*
* @brief ���@
*
* @date 2015/05/08
*
* @author ���V�@�L��
*/
#pragma once
#include <d3d11_1.h>
#include <SimpleMath.h>
#include <Keyboard.h>
#include "Obj3D.h"
#include <vector>
#include "CollisionNode.h"

// ���@
class Player
{
public:
	// �W�����v����
	static const float JUMP_SPEED_FIRST;
	static const float JUMP_SPEED_MAX;
	// �d�͉���
	static const float GRAVITY_ACC;

	// ���{�b�g�p�[�c
	enum
	{
		PARTS_TANK, // �L���^�s��
		PARTS_WAIST, // ��
		PARTS_BREAST, // ��
		PARTS_HEAD, // ��
		PARTS_ARM_R, // �E�r
		PARTS_GUN_R, // �E�̏e
		PARTS_NUM
	};
	// �R���X�g���N�^
	Player(DirectX::Keyboard* keyboard);
	// �f�X�g���N�^
	~Player();
	// ������
	void Initialize();
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
	// ���[���h�s����擾
	const DirectX::SimpleMath::Matrix& GetLocalWorld();
	void StartJump();
	void StartFall();
	void StopJump();
	// ���̓}�l�[�W����ݒ�
	const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }
	const DirectX::SimpleMath::Vector3& GetVelocity() {	return m_Velocity; }
protected:
	// �L�[�{�[�h
	DirectX::Keyboard* m_pKeyboard;
	// �����o�ϐ�
	// ���{�b�g�̍\���p�[�c
	std::vector<Obj3D> m_Obj;
	// �����苅
	SphereNode m_CollisionNodeBody;
	// ���x
	DirectX::SimpleMath::Vector3 m_Velocity;
	// �W�����v�i�����j�����H
	bool m_isJump;

};