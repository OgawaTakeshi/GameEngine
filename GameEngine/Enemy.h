//--------------------------------------------------------------------------------------
// ファイル名: Enemy.h
// 作成者:
// 作成日:
// 説明:
//--------------------------------------------------------------------------------------

#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <vector>
#include "Obj3D.h"
//#include "CollisionNode.h"

class Enemy
{
public:
	// パーツ
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

	Enemy();
	~Enemy();

	// 初期化
	void Initialize();
	// 毎フレーム更新
	void Update();
	// 行列の計算
	void Calc();
	// 描画
	void Draw();

	// 弾丸の当たり判定球を取得
	//const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }

	//// 死亡フラグを立てる
	//void SetDeath()	{ m_Death = true; }
	//bool GetDeath() { return m_Death; }

protected:
	// メンバ変数
	// ３Ｄオブジェクト
	std::vector<Obj3D>	m_Obj;

	//SphereNode m_CollisionNodeBody;

	// 角度
	float m_angle;
	// タイマー
	int m_Timer;

	// ギミックの稼動サイクル
	float m_cycle;
	// 死亡フラグ
	bool m_Death;
};
