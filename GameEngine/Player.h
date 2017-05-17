/**
* @file Player.h
*
* @brief 自機
*
* @date 2015/05/08
*
* @author 藤澤　伸治
*/
#pragma once
#include <d3d11_1.h>
#include <SimpleMath.h>
#include <Keyboard.h>
#include "Obj3D.h"
#include <vector>
#include "CollisionNode.h"

// 自機
class Player
{
public:
	// ジャンプ初速
	static const float JUMP_SPEED_FIRST;
	static const float JUMP_SPEED_MAX;
	// 重力加速
	static const float GRAVITY_ACC;

	// ロボットパーツ
	enum
	{
		PARTS_TANK, // キャタピラ
		PARTS_WAIST, // 腰
		PARTS_BREAST, // 胸
		PARTS_HEAD, // 頭
		PARTS_ARM_R, // 右腕
		PARTS_GUN_R, // 右の銃
		PARTS_NUM
	};
	// コンストラクタ
	Player(DirectX::Keyboard* keyboard);
	// デストラクタ
	~Player();
	// 初期化
	void Initialize();
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
	// ワールド行列を取得
	const DirectX::SimpleMath::Matrix& GetLocalWorld();
	void StartJump();
	void StartFall();
	void StopJump();
	// 入力マネージャを設定
	const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }
	const DirectX::SimpleMath::Vector3& GetVelocity() {	return m_Velocity; }
protected:
	// キーボード
	DirectX::Keyboard* m_pKeyboard;
	// メンバ変数
	// ロボットの構成パーツ
	std::vector<Obj3D> m_Obj;
	// 当たり球
	SphereNode m_CollisionNodeBody;
	// 速度
	DirectX::SimpleMath::Vector3 m_Velocity;
	// ジャンプ（落下）中か？
	bool m_isJump;

};