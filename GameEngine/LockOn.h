#pragma once

#include "Player.h"
#include "Enemy.h"

// ロックオン
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
	// 自機
	Player* m_Player;
	// 敵機
	std::vector<std::unique_ptr<Enemy>>* m_Enemies;
	// ロックオン対象のシリアルナンバー
	unsigned int m_LockOnSerial;
	// ロックオン中
	bool m_isLockOn;
};