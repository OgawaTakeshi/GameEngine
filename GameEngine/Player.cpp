//--------------------------------------------------------------------------------------
// �t�@�C����: Player.cpp
// �쐬��:
// �쐬��:
// ����:
//--------------------------------------------------------------------------------------

#include "Player.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

const float Player::GRAVITY_ACC = 0.03f;
const float Player::JUMP_SPEED_FIRST = 0.5f;
const float Player::JUMP_SPEED_MAX = 0.3f;

Player::Player(DirectX::Keyboard* keyboard)
: m_cycle(0.0f)
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

	m_FireFlag = false;

	m_CollisionNodeBody.Initialize();
	m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	m_CollisionNodeBody.SetLocalRadius(1.0f);
	m_CollisionNodeBody.SetTrans(Vector3(0, 1.0f, 0));

	// ������
	m_CollisionNodeBullet.Initialize();
	// �e�s���ݒ�
	m_CollisionNodeBullet.SetParentMatrix(&m_Obj[PARTS_GUN_R].GetLocalWorld());
	// �J�v�Z���̔��a
	m_CollisionNodeBullet.SetLocalRadius(0.3f);
	// �J�v�Z���̎�����
	m_CollisionNodeBullet.SetLength(1.4f);
	// �e�s�񂩂�̈ʒu
	m_CollisionNodeBullet.SetTrans(Vector3(0, 0, 0));
	// �e�s�񂩂�̉�]
	m_CollisionNodeBullet.SetRot(Vector3(-XM_PIDIV2, 0, 0));

	m_isJump = false;
}

//-----------------------------------------------------------------------------
// �X�V
//-----------------------------------------------------------------------------
void Player::Update()
{
	Keyboard::State keystate = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keystate);

	//if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::Space))
	//{
	//	StartJump();
	//}
	//if (m_isJump)
	//{
	//	m_Velocity.y -= GRAVITY_ACC;
	//	if (m_Velocity.y < -JUMP_SPEED_MAX)
	//	{
	//		m_Velocity.y = -JUMP_SPEED_MAX;
	//	}
	//}

	Vector3 pos = m_Obj[PARTS_TANK].GetTrans();
	pos += m_Velocity;
	m_Obj[PARTS_TANK].SetTrans(pos);

	// �P�t���[���ł̐��񑬓x<���W�A��>
	const float ROT_SPEED = 0.03f;

	

	// ����]/����]
	if (keystate.IsKeyDown(Keyboard::Keys::Space))
	{
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(Vector3(0,0,0));
	}

	// ����]/����]
	if (keystate.IsKeyDown(Keyboard::Keys::Q))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x -= ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::E))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x += ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	// ����]/����]
	if (keystate.IsKeyDown(Keyboard::Keys::A))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	// ����]/����]
	if (keystate.IsKeyDown(Keyboard::Keys::Z))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.z -= ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::C))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.z += ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();

		Game::m_debugText->AddText(Vector2(20, 20), L"Q E:Rotate X ... %+04d", (int)XMConvertToDegrees(rot.x));
		Game::m_debugText->AddText(Vector2(20, 40), L"A D:Rotate Y ... %+04d", (int)XMConvertToDegrees(rot.y));
		Game::m_debugText->AddText(Vector2(20, 60), L"Z C:Rotate Z ... %+04d", (int)XMConvertToDegrees(rot.z));
		Game::m_debugText->AddText(Vector2(20, 80), L"Space: Reset");
	}

	

	// �ړ��𔽉f���čs��X�V
	Calc();

	// �������������蔻��
	//{
	//	Sphere sphere = GetCollisionNodeBody();
	//	Vector3 trans = GetTrans();
	//	Vector3 sphere2player = trans - sphere.center;

	//	for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray->begin();
	//		it != m_pLandShapeArray->end();
	//		it++)
	//	{
	//		LandShape* pLandShape = it->get();

	//		Vector3 reject;	// �r�˃x�N�g�������邽�߂̕ϐ�

	//		if (pLandShape->IntersectSphere(sphere, &reject))
	//		{
	//			// �߂荞�ݕ������A���������o���悤�Ɉړ�
	//			sphere.center = sphere.center + reject;
	//		}
	//	}

	//	SetTrans(sphere.center + sphere2player);

	//	Calc();
	//}

	// ���������n�`�����蔻��
	//{
	//	const Vector3& vel = GetVelocity();

	//	if (vel.y <= 0.0f)
	//	{
	//		bool hit = false;
	//		Segment player_segment;
	//		Vector3 player_pos = GetTrans();
	//		player_segment.start = player_pos + Vector3(0, 1.0f, 0);
	//		player_segment.end = player_pos + Vector3(0, -0.5f, 0);

	//		// �傫�������ŏ�����
	//		float distance = 1.0e5;
	//		Vector3 inter;

	//		for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray->begin();
	//			it != m_pLandShapeArray->end();
	//			it++)
	//		{
	//			LandShape* pLandShape = it->get();
	//			float temp_distance;
	//			Vector3 temp_inter;

	//			// ���ʂƂ̓�����𔻒�
	//			if (pLandShape->IntersectSegment(player_segment, &temp_inter))
	//			{
	//				hit = true;
	//				temp_distance = Vector3::Distance(player_segment.start, temp_inter);
	//				if (temp_distance < distance)
	//				{
	//					inter = temp_inter;
	//					distance = temp_distance;
	//				}
	//			}
	//		}

	//		if (hit)
	//		{
	//			Vector3 new_position = player_pos;
	//			new_position.y = inter.y;
	//			StopJump();
	//			SetTrans(new_position);
	//		}

	//		if (!hit)
	//		{// �����J�n
	//			StartFall();
	//		}
	//	}

	//	Calc();
	//}

	
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

	m_CollisionNodeBody.Update();
	// �����蔻��̍X�V�i�e�̍s��X�V��ɍs�����Ɓj
	m_CollisionNodeBullet.Update();
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

	m_CollisionNodeBody.Draw();
	// �����蔻��̃f�o�b�O�\��
	m_CollisionNodeBullet.Draw();
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

void Player::SetRot(const DirectX::SimpleMath::Vector3& rot)
{
	// �^���N�p�[�c�̍��W��ݒ�
	m_Obj[PARTS_TANK].SetRot(rot);
}

const DirectX::SimpleMath::Matrix& Player::GetLocalWorld()
{
	// �^���N�p�[�c�̃��[���h�s���Ԃ�
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

// �e�ۗp�̃p�[�c���ˏo����
void Player::FireBullet()
{
	// ���ɔ��˒�
	if (m_FireFlag)	return;

	m_FireFlag = true;

	// �e�q�֌W�������������[���h���W���擾
	Matrix worldm = m_Obj[PARTS_GUN_R].GetLocalWorld();

	Vector3 scale;
	Quaternion rotq;
	Vector3 pos;
	// �s��̂P�s�����AVector3�Ƃ��Ĉ���
	Vector3* m0 = (Vector3*)&worldm.m[0];
	Vector3* m1 = (Vector3*)&worldm.m[1];
	Vector3* m2 = (Vector3*)&worldm.m[2];
	Vector3* m3 = (Vector3*)&worldm.m[3];
	// ���[���h���W�����o��
	pos = *m3;
	// ���[���h�X�P�[�����O�����o��
	scale = Vector3(m0->Length(), m1->Length(), m2->Length());
	// �X�P�[�����O��ł���������������Ȃ��ƁA�N�H�[�^�j�I�������������Ȃ�
	m0->Normalize();
	m1->Normalize();
	m2->Normalize();
	// ���[���h��]�����o��
	rotq = Quaternion::CreateFromRotationMatrix(worldm);

	// �ꔭ�ł���Ă����֐�
	//worldm.Decompose(scale, rotq, pos);

	// �e�q�֌W����������
	m_Obj[PARTS_GUN_R].SetParentMatrix(nullptr);
	m_Obj[PARTS_GUN_R].SetScale(scale);
	m_Obj[PARTS_GUN_R].SetRotQ(rotq);
	m_Obj[PARTS_GUN_R].SetTrans(pos);

	// ���˂���e�ۂ̑��x�x�N�g��
	m_BulletVel = Vector3(0, 0.0f,-0.1f);
	// �x�N�g�����N�H�[�^�j�I���ŉ�]
	m_BulletVel = Vector3::Transform(m_BulletVel, rotq);

	// �����I�Ɏ��t���Ȃ����ׂ̃J�E���g�_�E��
	m_FireCount = 120;
}

// �e�ۗp�̃p�[�c�����{�b�g�Ɏ��t���Ȃ���
void Player::ResetBullet()
{
	// ���˒��ł͂Ȃ�
	if (!m_FireFlag)	return;
	
	m_Obj[PARTS_GUN_R].SetParentMatrix(&m_Obj[PARTS_ARM_R].GetLocalWorld());

	m_Obj[PARTS_GUN_R].SetScale(Vector3(1, 1, 1));
	m_Obj[PARTS_GUN_R].SetRot(Vector3(0, 0, 0));
	m_Obj[PARTS_GUN_R].SetTrans(Vector3(0.2f, 0.45f, -0.3f));

	m_FireFlag = false;
}

// �W�����v���J�n����
void Player::StartJump()
{
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
void Player::StartFall()
{
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
void Player::StopJump()
{
	// �W�����v�t���O�����낷
	m_isJump = false;
	// �f�t�H���g�l�ŏ�����
	m_Velocity = Vector3();
}