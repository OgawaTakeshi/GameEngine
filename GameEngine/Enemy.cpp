//--------------------------------------------------------------------------------------
// �t�@�C����: Enemy.h
// �쐬��:
// �쐬��:
// ����:
//--------------------------------------------------------------------------------------

#include "Enemy.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/*==================================
�ړI�̊p�x�ւ̍ŒZ�p�x���擾�i���W�A���j

����	_angle0	�x�[�X�ƂȂ�p�x
_angle1	�ڕW�Ƃ���p�x
�߂�l	�����p�x

�p�x�O����p�x�P�ɍŒZ�R�[�X�Ō������ۂɉ��Z����p�x���擾����
===================================*/
static float GetShortAngleRad(float _angle0, float _angle1)
{
	float angle_sub;

	angle_sub = _angle1 - _angle0;	// �p�x�̍�
	// �����P�W�O�x(�΁j���傫�������ꍇ�A�t��]�̕����߂��̂ŁA�}�C�i�X�ɕϊ�
	// �ŏI�I��-180�`+180�x�͈̔͂ɁB
	if (angle_sub > XM_PI)
	{
		angle_sub -= XM_2PI;
	}
	else if (angle_sub < -XM_PI)
	{
		angle_sub += XM_2PI;
	}

	return angle_sub;
}

//-----------------------------------------------------------------------------
// �R���X�g���N�^
//-----------------------------------------------------------------------------
Enemy::Enemy()
: m_cycle(0.0f)
, m_Death(false)
{
	m_Timer = 60;
}

//-----------------------------------------------------------------------------
// �f�X�g���N�^
//-----------------------------------------------------------------------------
Enemy::~Enemy()
{
}

//-----------------------------------------------------------------------------
// ������
//-----------------------------------------------------------------------------
void Enemy::Initialize()
{
	m_Obj.resize(PARTS_NUM);
	// ���f���ǂݍ���
	m_Obj[PARTS_TANK].LoadModelFile(L"Resources/tank.cmo");
	m_Obj[PARTS_WAIST].LoadModelFile(L"Resources/waist.cmo");
	m_Obj[PARTS_BREAST].LoadModelFile(L"Resources/breast.cmo");
	m_Obj[PARTS_HEAD].LoadModelFile(L"Resources/head.cmo");
	m_Obj[PARTS_ARM_R].LoadModelFile(L"Resources/arm.cmo");
	m_Obj[PARTS_GUN_R].LoadModelFile(L"Resources/gun.cmo");

	// �e�q�֌W�̑g�ݗ���
	m_Obj[PARTS_WAIST].SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	m_Obj[PARTS_BREAST].SetParentMatrix(&m_Obj[PARTS_WAIST].GetLocalWorld());
	m_Obj[PARTS_HEAD].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());
	m_Obj[PARTS_ARM_R].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());
	m_Obj[PARTS_GUN_R].SetParentMatrix(&m_Obj[PARTS_ARM_R].GetLocalWorld());

	// �e����̃I�t�Z�b�g�i�ʒu�̍��j��ݒ�
	m_Obj[PARTS_TANK].SetTrans(Vector3(0, 0.01f, 0));
	m_Obj[PARTS_WAIST].SetScale(Vector3(0.7f, 0.7f, 0.7f));
	m_Obj[PARTS_WAIST].SetTrans(Vector3(0, 0.40f, 0));
	m_Obj[PARTS_BREAST].SetTrans(Vector3(0, 0.5f, 0));
	m_Obj[PARTS_HEAD].SetTrans(Vector3(0, 0.42f, 0));
	m_Obj[PARTS_ARM_R].SetTrans(Vector3(0.5f, 0.3f, 0));
	m_Obj[PARTS_GUN_R].SetTrans(Vector3(0.2f, 0.45f, -0.3f));

	Vector3 pos;
	pos.x = (float)rand() / RAND_MAX * 20.0f - 10.0f;
	pos.z = (float)rand() / RAND_MAX * 20.0f - 10.0f;
	m_Obj[PARTS_TANK].SetTrans(pos);

	m_angle = (float)(rand() % 360);

	m_Obj[PARTS_TANK].SetRot(Vector3(0, XMConvertToRadians(m_angle), 0));

	//m_CollisionNodeBody.Initialize();
	//m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	//m_CollisionNodeBody.SetTrans(Vector3(0, 0.7f, 0));
	//m_CollisionNodeBody.SetLocalRadius(0.7f);
}

//-----------------------------------------------------------------------------
// �X�V
//-----------------------------------------------------------------------------
void Enemy::Update()
{
	// ����ł����牽�����Ȃ�
	if (m_Death) return;

	// ����I�ɐi�s������ς���
	m_Timer--;
	if (m_Timer < 0)
	{
		m_Timer = 60;

		// �p�x��ύX
		float rnd = (float)rand() / RAND_MAX - 0.5f;
		rnd *= 180.0f;
		m_angle += rnd;
	}

	// ���킶��Ɗp�x�𔽉f
	{
		Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
		float angle = GetShortAngleRad(rotv.y, XMConvertToRadians(m_angle));
		rotv.y += angle*0.01f;
		m_Obj[PARTS_TANK].SetRot(rotv);
	}

	// �@�̂̌����Ă�������ɐi��
	{
		// ���̍��W���擾
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();

		Vector3 move(0, 0, -0.02f);
		Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
		Matrix rotm = Matrix::CreateRotationY(rotv.y);
		move = Vector3::TransformNormal(move, rotm);
		// ���W���ړ�
		trans += move;
		// �ړ���̍��W���Z�b�g
		m_Obj[PARTS_TANK].SetTrans(trans);
	}

	// �ړ��𔽉f���čs��X�V
	Calc();

	//m_CollisionNodeBody.Update();
}

//-----------------------------------------------------------------------------
// �s��X�V
//-----------------------------------------------------------------------------
void Enemy::Calc()
{
	// ����ł����牽�����Ȃ�
	if (m_Death) return;

	// �S�p�[�c���s��X�V
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Calc();
	}
}

//-----------------------------------------------------------------------------
// �`��
//-----------------------------------------------------------------------------
void Enemy::Draw()
{
	// ����ł����牽�����Ȃ�
	if (m_Death) return;

	// �S�p�[�c���`��
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Draw();
	}

	//m_CollisionNodeBody.Draw();
}
