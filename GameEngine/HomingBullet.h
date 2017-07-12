/// <summary>
/// �U�����\�t���̒e
/// </summary>
#pragma once

#include "Obj3D.h"

class Enemy;

class HomingBullet
{
public:
	// ��s���x<m/frame>
	static const float SPEED;
	// 1�t���[���ł̊p�x�ω�����<�x�����W�A��>
	static const float ANGLE_DELTA_MAX;
	// ����
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
	// 3D�I�u�W�F�N�g
	Obj3D m_Obj;
	// ���x
	DirectX::SimpleMath::Vector3 m_Velocity;
	// �U���Ώ�
	Enemy* m_Target;
	// �c�����
	int m_LifeTime;
};

