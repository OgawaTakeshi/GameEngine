//--------------------------------------------------------------------------------------
// �t�@�C����: Player.cpp
// �쐬��:
// �쐬��:
// ����:
//--------------------------------------------------------------------------------------
#include "Player.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// static�ϐ��̎���
const float Player::GRAVITY_ACC = 0.03f;
const float Player::JUMP_SPEED_FIRST = 0.5f;
const float Player::JUMP_SPEED_MAX = 0.3f;


Player::Player(DirectX::Keyboard* keyboard)
{
	m_pKeyboard = keyboard;

	Initialize();
}

Player::~Player()
{
}

//-----------------------------------------------------------------------------
// ������
//-----------------------------------------------------------------------------
void Player::Initialize()
{
	m_Obj.resize(PARTS_NUM);
	// ���{�b�g�̃��f���ǂݍ���
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

	// �����蔻��m�[�h�̐ݒ�
	{
		// ������
		m_CollisionNodeBody.Initialize();
		// �e�s���ݒ�
		m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
		// ���̔��a
		m_CollisionNodeBody.SetLocalRadius(0.7f);
		// �e�s�񂩂�̈ʒu
		m_CollisionNodeBody.SetTrans(Vector3(0, 0.7f, 0));
	}

}
//-----------------------------------------------------------------------------
// �X�V
//-----------------------------------------------------------------------------
void Player::Update()
{
	Keyboard::State keystate = m_pKeyboard->GetState();

	// �P�t���[���ł̐��񑬓x<���W�A��>
	const float ROT_SPEED = 0.03f;
	// ����]/����]
	if (keystate.A)
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.D)
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	//if (g_key.Up)
	//{
	//	// ���݂̊p�x���擾
	//	Vector3 rot = m_Obj[PARTS_TANK].GetRot();
	//	rot.x += ROT_SPEED;
	//	// ��]��̊p�x�𔽉f
	//	m_Obj[PARTS_TANK].SetRot(rot);
	//}
	//if (g_key.Down)
	//{
	//	// ���݂̊p�x���擾
	//	Vector3 rot = m_Obj[PARTS_TANK].GetRot();
	//	rot.x -= ROT_SPEED;
	//	// ��]��̊p�x�𔽉f
	//	m_Obj[PARTS_TANK].SetRot(rot);
	//}
	// �O�i/���
	if (keystate.W)
	{
		// ���݂̍��W�E��]�p���擾
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		//float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// �ړ��x�N�g��(Z���W�O�i)
		//SimpleMath::Vector3 moveV(0, 0, -0.1f);
		SimpleMath::Vector3 moveV(0, 0,-0.1f);
		//Matrix rotm = Matrix::CreateRotationY(rot_y);
		//Matrix rotm = Matrix::CreateFromYawPitchRoll(rot.y, rot.x, rot.z);
		const Matrix& rotm = GetLocalWorld();
		// �ړ��x�N�g������]����
		moveV = Vector3::TransformNormal(moveV, rotm);
		// �ړ�
		trans += moveV;
		// �ړ��������W�𔽉f
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	if (keystate.S)
	{
		// ���݂̍��W�E��]�p���擾
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// �ړ��x�N�g��(Z���W���)
		//Vector3 moveV(0, 0, +0.1f);
		SimpleMath::Vector3 moveV(0, 0,0.1f);
		//Matrix rotm = Matrix::CreateRotationY(rot_y);
		const Matrix& rotm = GetLocalWorld();
		// �ړ��x�N�g������]����
		moveV = Vector3::TransformNormal(moveV, rotm);
		// �ړ�
		trans += moveV;
		// �ړ��������W�𔽉f
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	//// ����]
	//..�ȗ�..
	//	// ����]
	//	..�ȗ�..
	//	// �����E��]
	//	..�ȗ�..
	//	// ���㉺��]
	//	..�ȗ�..
	//	// �r���E��]
	//	..�ȗ�..
	//	// �r�㉺��]
	//	..�ȗ�..
	//	// �e���E��]
	//	..�ȗ�..

	//if (m_isJump)
	//{
	//	m_Velocity += Vector3(0, -0.01, 0);

	//	// ���݂̍��W�E��]�p���擾
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();

	//	trans += m_Velocity;

	//	m_Obj[PARTS_TANK].SetTrans(trans);
	//}

	// �ړ��𔽉f���čs��X�V
	Calc();
}
//-----------------------------------------------------------------------------
// �s��X�V
//-----------------------------------------------------------------------------
void Player::Calc()
{
	// �S�p�[�c���s��X�V
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Calc();
	}

	// �����蔻��̍X�V�i�e�̍s��X�V��ɍs�����Ɓj
	m_CollisionNodeBody.Update();
}
//-----------------------------------------------------------------------------
// �`��
//-----------------------------------------------------------------------------
void Player::Draw()
{
	// �S�p�[�c���`��
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Draw();
	}

	// �����蔻��̃f�o�b�O�\��
	m_CollisionNodeBody.Draw();

}
const DirectX::SimpleMath::Vector3& Player::GetTrans()
{
	// �^���N�p�[�c�̍��W��Ԃ�
	return m_Obj[PARTS_TANK].GetTrans();
}
void Player::SetTrans(const DirectX::SimpleMath::Vector3& trans)
{
	// �^���N�p�[�c�̍��W��ݒ�
	m_Obj[PARTS_TANK].SetTrans(trans);
}
const DirectX::SimpleMath::Matrix& Player::GetLocalWorld()
{
	// �^���N�p�[�c�̃��[���h�s���Ԃ�
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

// �W�����v���J�n����
void Player::StartJump() {
	// �W�����v���łȂ���
	if (!m_isJump)
	{
		// ������̏�����ݒ�
		m_Velocity.y = JUMP_SPEED_FIRST;
		// �W�����v�t���O�𗧂Ă�
		m_isJump = true;
	}
}

// �������J�n����
void Player::StartFall() {
	// �W�����v���łȂ���
	if (!m_isJump)
	{
		// ������̏�����0
		m_Velocity.y = 0.0f;
		// �W�����v�t���O�𗧂Ă�
		m_isJump = true;
	}
}

// �W�����v���I������
void Player::StopJump() {
	// �W�����v�t���O�����낷
	m_isJump = false;
	// �f�t�H���g�l�ŏ�����
	m_Velocity = Vector3();
}
