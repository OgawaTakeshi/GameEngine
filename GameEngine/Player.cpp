//--------------------------------------------------------------------------------------
// �t�@�C����: Player.cpp
// �쐬��:
// �쐬��:
// ����:
//--------------------------------------------------------------------------------------

#include "Player.h"
#include <fstream>
#include <sstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

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
	Load();

	m_FireFlag = false;

	m_CollisionNodeBody.Initialize();
	m_CollisionNodeBody.SetParent(&m_Obj[PARTS_TANK]);
	m_CollisionNodeBody.SetLocalRadius(1.0f);
	m_CollisionNodeBody.SetTrans(Vector3(0, 1.0f, 0));

	// ������
	m_CollisionNodeBullet.Initialize();
	// �e�s���ݒ�
	m_CollisionNodeBullet.SetParent(&m_Obj[PARTS_GUN_R]);
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
	if (keystate.IsKeyDown(Keyboard::Keys::A))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D))
	{
		// ���݂̊p�x���擾
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		// ��]��̊p�x�𔽉f
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	// �O�i/���
	if (keystate.IsKeyDown(Keyboard::Keys::W))
	{
		// ���݂̍��W�E��]�p���擾
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// �ړ��x�N�g��(Z���W�O�i)
		SimpleMath::Vector3 moveV(0, 0, -0.1f);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		// �ړ��x�N�g������]����
		moveV = Vector3::TransformNormal(moveV, rotm);
		// �ړ�
		trans += moveV;
		// �ړ��������W�𔽉f
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::S))
	{
		// ���݂̍��W�E��]�p���擾
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// �ړ��x�N�g��(Z���W���)
		Vector3 moveV(0, 0, +0.1f);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		// �ړ��x�N�g������]����
		moveV = Vector3::TransformNormal(moveV, rotm);
		// �ړ�
		trans += moveV;
		// �ړ��������W�𔽉f
		m_Obj[PARTS_TANK].SetTrans(trans);
	}

	// �㏸/���~
	//if (m_pInputManager->GetKeyboard()->IsKeyDown(VK_UP))
	//{
	//	// ���݂̍��W�E��]�p���擾
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
	//	// �ړ��x�N�g��(�㏸)
	//	SimpleMath::Vector3 moveV(0, 0.1f, 0);
	//	trans += moveV;
	//	// �ړ��������W�𔽉f
	//	m_Obj[PARTS_TANK].SetTrans(trans);
	//}
	//if (m_pInputManager->GetKeyboard()->IsKeyDown(VK_DOWN))
	//{
	//	// ���݂̍��W�E��]�p���擾
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
	//	// �ړ��x�N�g��(Z���W���)
	//	Vector3 moveV(0, -0.1f, 0);
	//	// �ړ�
	//	trans += moveV;
	//	// �ړ��������W�𔽉f
	//	m_Obj[PARTS_TANK].SetTrans(trans);
	//}

	// ����]
	if (keystate.IsKeyDown(Keyboard::Keys::Z))
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::X))
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}

	// ����]
	if (keystate.IsKeyDown(Keyboard::Keys::C))
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::V))
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}

	// �����E��]
	if (keystate.IsKeyDown(Keyboard::Keys::U))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::I))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}

	// ���㉺��]
	if (keystate.IsKeyDown(Keyboard::Keys::O))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.x -= 0.03f;
		if (rot.x < -0.1f)
		{
			rot.x = -0.1f;
		}
		m_Obj[PARTS_HEAD].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::P))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.x += 0.03f;
		if (rot.x > 0.5f)
		{
			rot.x = 0.5f;
		}
		m_Obj[PARTS_HEAD].SetRot(rot);
	}

	// �r���E��]
	if (keystate.IsKeyDown(Keyboard::Keys::D7))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.y += 0.03f;
		if (rot.y > 0.0f)
		{
			rot.y = 0.0f;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D8))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.y -= 0.03f;
		if (rot.y < -XM_PIDIV2)
		{
			rot.y = -XM_PIDIV2;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}

	// �r�㉺��]
	if (keystate.IsKeyDown(Keyboard::Keys::D9))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.x -= 0.03f;
		if (rot.x < -XM_PIDIV4)
		{
			rot.x = -XM_PIDIV4;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D0))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.x += 0.03f;
		if (rot.x > XM_PIDIV4)
		{
			rot.x = XM_PIDIV4;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}

	// �e���E��]
	if (keystate.IsKeyDown(Keyboard::Keys::J))
	{
		Vector3 rot = m_Obj[PARTS_GUN_R].GetRot();
		rot.y += 0.03f;
		if (rot.y > XM_PIDIV4)
		{
			rot.y = XM_PIDIV4;
		}
		m_Obj[PARTS_GUN_R].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::K))
	{
		Vector3 rot = m_Obj[PARTS_GUN_R].GetRot();
		rot.y -= 0.03f;
		if (rot.y < -XM_PIDIV2)
		{
			rot.y = -XM_PIDIV2;
		}
		m_Obj[PARTS_GUN_R].SetRot(rot);
	}

	// �����o�ϐ��ŃT�C���p�̊p�x�����Z��������
	m_cycle += 0.1f;
	// 1 + (-1~+1)�͈̔́@�܂�(0~2)�{�̃X�P�[�����O�ŕϓ�
	float scale = 1.0f + sinf(m_cycle);
	// �p�[�c�ɃX�P�[�����O�̐��l��ݒ肷��
	m_Obj[PARTS_HEAD].SetScale(Vector3(scale));

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::E))
	{
		if (m_FireFlag)
		{
			ResetBullet();
		}
		else
		{
			// ����
			FireBullet();
		}
	}

	if (m_FireFlag)
	{
		// ���݂̍��W���擾
		Vector3 trans = m_Obj[PARTS_GUN_R].GetTrans();

		trans += m_BulletVel;
		
		// �ړ��������W�𔽉f
		m_Obj[PARTS_GUN_R].SetTrans(trans);

		if (--m_FireCount < 0)
		{
			ResetBullet();
		}
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

void Player::Load()
{
	wifstream ifs("CSV/Player.csv");

	wstring line;

	// 1�s�ڂ��X�L�b�v
	getline(ifs, line);

	vector<wstring> part_names;
	vector<wstring> parent_names;

	// ���납��f�[�^�����
	while (getline(ifs, line)) {

		wstring filepath;
		wstring filename;

		// 1�s���𕶎���X�g���[���ɕϊ�
		wistringstream stream(line);

		// �t�@�C�����̕������ǂݍ���
		stream >> filename;
		// �v���W�F�N�g�̊�_����̑��΃p�X�Ńt�@�C������₤
		filepath = L"Resources/" + filename + L".cmo";

		Obj3D obj;
		// �ǂݍ���
		obj.LoadModelFile(filepath.c_str());

		// �X�P�[�����O�̓ǂݎ��
		Vector3 scale;
		stream >> scale.x;
		stream >> scale.y;
		stream >> scale.z;
		obj.SetScale(scale);

		// �p�x�̓ǂݎ��
		Vector3 rotation;
		stream >> rotation.z;
		stream >> rotation.x;
		stream >> rotation.y;
		obj.SetRot(rotation);

		// ���W�̓ǂݎ��
		Vector3 position;
		stream >> position.x;
		stream >> position.y;
		stream >> position.z;
		obj.SetTrans(position);

		m_Obj.push_back(obj);

		wstring parent_name;

		stream >> parent_name;

		part_names.push_back(filename);

		parent_names.push_back(parent_name);
	}

	// �e�q�֌W�̑g�ݗ���
	for (int i = 0; i < m_Obj.size(); i++)
	{
		// �e�̎w�肠��
		if (parent_names[i].length() > 0)
		{
			for (int j = 0; j < m_Obj.size(); j++)
			{
				if (j == i) continue;

				// �w��̐e����
				if (part_names[j] == parent_names[i])
				{
					m_Obj[i].SetParent(&m_Obj[j]);
				}
			}
		}
	}
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
	m_Obj[PARTS_GUN_R].SetParent(nullptr);
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
	
	m_Obj[PARTS_GUN_R].SetParent(&m_Obj[PARTS_ARM_R]);

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