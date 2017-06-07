//--------------------------------------------------------------------------------------
// �t�@�C����: Enemy.h
// �쐬��:
// �쐬��:
// ����:
//--------------------------------------------------------------------------------------

#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>
#include "Obj3D.h"
#include "CollisionNode.h"

class Enemy
{
public:
	// �p�[�c
	// ���@�p�[�c
	enum PARTS
	{
		PARTS_BODY,
		PARTS_COCKPIT,
		PARTS_LAUNCHER,
		PARTS_SHIELD,
		PARTS_DRILL,

		PARTS_NUM
	};

	Enemy();
	~Enemy();

	// ������
	void Initialize();
	// ���t���[���X�V
	void Update();
	// �s��̌v�Z
	void Calc();
	// �`��
	void Draw();

	// �e�ۂ̓����蔻�苅���擾
	const CollisionNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }

	// ���S�t���O�𗧂Ă�
	void SetDeath()	{ m_Death = true; }
	bool GetDeath() { return m_Death; }

protected:
	// �����o�ϐ�
	// �R�c�I�u�W�F�N�g
	std::vector<Obj3D>	m_Obj;

	SphereNode m_CollisionNodeBody;

	// �p�x
	float m_angle;
	// �^�C�}�[
	int m_Timer;

	// �M�~�b�N�̉ғ��T�C�N��
	float m_cycle;
	// ���S�t���O
	bool m_Death;
};
