/**
*	@file	Player.h
*
*	@brief	自機
*
*	@date	2015/05/08
*
*	@author	藤澤　伸治
*/
#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>
#include "Obj3D.h"
#include "CollisionNode.h"
#include "LandShape.h"

// 自機
class Player
{
public:
	// ロボットパーツ
	enum
	{
		PARTS_TANK,		// キャタピラ
		PARTS_WAIST,	// 腰
		PARTS_BREAST,	// 胸
		PARTS_HEAD,		// 頭
		PARTS_ARM_R,	// 右腕
		PARTS_GUN_R,	// 右の銃

		PARTS_NUM
	};

	// ジャンプ初速
	static const float JUMP_SPEED_FIRST;
	static const float JUMP_SPEED_MAX;
	// 重力加速
	static const float GRAVITY_ACC;

	// コンストラクタ
	Player();
	// デストラクタ
	~Player();
	// 初期化
	void Initialize();
	void ControlNormal();
	void ControlLockOn();
	// 毎フレーム更新
	void Update();
	// 行列の計算
	void Calc();
	// 描画
	void Draw();
	// 座標を取得
	const DirectX::SimpleMath::Vector3& GetTrans();
	// 座標を設定
	void SetTrans(const DirectX::SimpleMath::Vector3& trans);
	// 回転を設定
	void SetRot(const DirectX::SimpleMath::Vector3& rot);
	// ワールド行列を取得
	const DirectX::SimpleMath::Matrix& GetLocalWorld();
	void Load();
	// 本体の地形当たり判定球を取得
	const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }
	// 弾丸の当たり判定カプセルを取得
	const SphereNode& GetCollisionNodeBullet() { return m_CollisionNodeBullet; }

	// 発射
	void FireBullet();
	// 戻す
	void ResetBullet();

	// 座標を取得
	const DirectX::SimpleMath::Vector3& GetVelocity() { return m_Velocity; }

	void StartJump();
	void StartFall();
	void StopJump();

	void SetLandShapeArray(std::vector<std::unique_ptr<LandShape>>*	pLandShapeArray) { m_pLandShapeArray = pLandShapeArray; }

protected:
	// メンバ変数
	// ロボットの構成パーツ
	std::vector<Obj3D>	m_Obj;

	// 影
	Obj3D m_ObjShadow;
	// ギミックの稼動サイクル
	float m_cycle;

	bool m_FireFlag;
	int m_FireCount;
	DirectX::SimpleMath::Vector3 m_BulletVel;

	// 本体の地形当たり判定球
	SphereNode m_CollisionNodeBody;

	// 弾丸の当たり判定球
	SphereNode m_CollisionNodeBullet;

	// 速度
	DirectX::SimpleMath::Vector3 m_Velocity;

	// ジャンプ（落下）中か？
	bool m_isJump;

	std::vector<std::unique_ptr<LandShape>>*	m_pLandShapeArray;
};

