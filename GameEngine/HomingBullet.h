/// <summary>
/// 誘導性能付きの弾
/// </summary>
#pragma once

#include "Obj3D.h"

class Enemy;

class HomingBullet
{
public:
	// 飛行速度<m/frame>
	static const float SPEED;
	// 1フレームでの角度変化制限<度→ラジアン>
	static const float ANGLE_DELTA_MAX;
	// 寿命
	static const int LIFE_TIME;

	HomingBullet();

	void Initialize();

	bool Update();

	void Draw();

	void Fire(const DirectX::SimpleMath::Vector3 & pos, const DirectX::SimpleMath::Vector3 & direction);

	//setter
	void SetVelocity(const DirectX::SimpleMath::Vector3& velocity) { m_Velocity = velocity; }
	void SetTarget(Enemy* target) { m_Target = target; }
protected:
	// 3Dオブジェクト
	Obj3D m_Obj;
	// 速度
	DirectX::SimpleMath::Vector3 m_Velocity;
	// 誘導対象
	Enemy* m_Target;
	// 残り寿命
	int m_LifeTime;
};

