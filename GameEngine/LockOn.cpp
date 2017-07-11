#include "LockOn.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

LockOn::LockOn()
{
	m_Player = nullptr;
	m_Enemies = nullptr;
	m_LockOnSerial = 0;
	m_isLockOn = false;
}

/// <summary>
/// ���@�Ɉ�ԋ������߂��G�����b�N�I��
/// </summary>
bool LockOn::Start()
{
	// ���@���Z�b�g
	if (m_Player == nullptr) return false;
	// �G�z�񖢃Z�b�g
	if (m_Enemies == nullptr) return false;
	// �G���S��
	if (m_Enemies->size() == 0) return false;

	// �ꎞ���
	struct Temporary
	{
		// ����
		float distance;
		// �z��̒��ł̏���
		int index;
	};
	// ���@���W
	const Vector3& playerPos = m_Player->GetTrans();

	Temporary temp;
	// �\���ɑ傫������
	temp.distance = 1.0e5f;
	temp.index = -1;

	// �S�Ă̓G�ɂ���
	int enemyNum = (int)m_Enemies->size();
	for (int i = 0; i < enemyNum; i++)
	{
		Enemy* enemy = (*m_Enemies)[i].get();
		const Vector3& enemyPos = enemy->GetTrans();
		// ���@�ƓG�@�̋���
		float distance = Vector3::Distance(playerPos, enemyPos);
		// �����������܂łň�ԋ߂�
		if (distance < temp.distance)
		{
			temp.distance = distance;
			temp.index = i;
		}
	}

	// �z��̗v�f�O�A�N�Z�X�����
	assert(0 <= temp.index && temp.index < enemyNum);

	// ��ԋ߂��G�����b�N�I��
	m_LockOnSerial = (*m_Enemies)[temp.index]->GetSerial();
	m_isLockOn = true;

	return true;
}

/// <summary>
/// ���b�N�I������
/// </summary>
void LockOn::End()
{
	m_isLockOn = false;
	m_LockOnSerial = 0;
}

Enemy * LockOn::SearchLockingEnemy()
{
	Enemy* lockingEnemy = nullptr;
	// ���b�N�I�����H
	if (IsLockOn())
	{
		// ���b�N�I�����̓G���������m�F
		int enemyNum = (int)m_Enemies->size();
		for (int i = 0; i < enemyNum; i++)
		{
			Enemy* enemy = (*m_Enemies)[i].get();

			// ����
			if (enemy->GetSerial() == m_LockOnSerial)
			{
				lockingEnemy = enemy;
				break;
			}
		}
	}

	if (lockingEnemy == nullptr)
	{
		// ���b�N�I������
		End();
	}

	return lockingEnemy;
}
