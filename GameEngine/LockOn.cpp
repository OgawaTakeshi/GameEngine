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
/// 自機に一番距離が近い敵をロックオン
/// </summary>
bool LockOn::Start()
{
	// 自機未セット
	if (m_Player == nullptr) return false;
	// 敵配列未セット
	if (m_Enemies == nullptr) return false;
	// 敵が全滅
	if (m_Enemies->size() == 0) return false;

	// 一時情報
	struct Temporary
	{
		// 距離
		float distance;
		// 配列の中での順番
		int index;
	};
	// 自機座標
	const Vector3& playerPos = m_Player->GetTrans();

	Temporary temp;
	// 十分に大きい距離
	temp.distance = 1.0e5f;
	temp.index = -1;

	// 全ての敵について
	int enemyNum = (int)m_Enemies->size();
	for (int i = 0; i < enemyNum; i++)
	{
		Enemy* enemy = (*m_Enemies)[i].get();
		const Vector3& enemyPos = enemy->GetTrans();
		// 自機と敵機の距離
		float distance = Vector3::Distance(playerPos, enemyPos);
		// 距離がここまでで一番近い
		if (distance < temp.distance)
		{
			temp.distance = distance;
			temp.index = i;
		}
	}

	// 配列の要素外アクセスを回避
	assert(0 <= temp.index && temp.index < enemyNum);

	// 一番近い敵をロックオン
	m_LockOnSerial = (*m_Enemies)[temp.index]->GetSerial();
	m_isLockOn = true;

	return true;
}

/// <summary>
/// ロックオン解除
/// </summary>
void LockOn::End()
{
	m_isLockOn = false;
	m_LockOnSerial = 0;
}

Enemy * LockOn::SearchLockingEnemy()
{
	Enemy* lockingEnemy = nullptr;
	// ロックオン中？
	if (IsLockOn())
	{
		// ロックオン中の敵が存命か確認
		int enemyNum = (int)m_Enemies->size();
		for (int i = 0; i < enemyNum; i++)
		{
			Enemy* enemy = (*m_Enemies)[i].get();

			// 発見
			if (enemy->GetSerial() == m_LockOnSerial)
			{
				lockingEnemy = enemy;
				break;
			}
		}
	}

	if (lockingEnemy == nullptr)
	{
		// ロックオン解除
		End();
	}

	return lockingEnemy;
}
