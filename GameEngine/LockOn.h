#pragma once

#include "Player.h"
#include "Enemy.h"

// ���b�N�I��
class LockOn
{
public:
	LockOn();
	bool Start();
	void End();
	Enemy* SearchLockingEnemy();
	// setter
	void SetPlayer(Player* player) { m_Player = player; }
	void SetEnemies(std::vector<std::unique_ptr<Enemy>>* enemies) { m_Enemies = enemies; }
	// getter
	Player* GetPlayer() { return m_Player; }
	bool IsLockOn() { return m_isLockOn; }
protected:
	// ���@
	Player* m_Player;
	// �G�@
	std::vector<std::unique_ptr<Enemy>>* m_Enemies;
	// ���b�N�I���Ώۂ̃V���A���i���o�[
	unsigned int m_LockOnSerial;
	// ���b�N�I����
	bool m_isLockOn;
};