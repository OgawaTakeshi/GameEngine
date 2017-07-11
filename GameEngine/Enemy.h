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
#include "CollisionNode.h"

class Enemy
{
private:
	// 通し番号カウント
	static unsigned int s_SerialCount;
public:
	// パーツ
	// 自機パーツ
	enum PARTS
	{
		PARTS_BODY,
		PARTS_COCKPIT,
		PARTS_LAUNCHER,
		PARTS_SHIELD,
		PARTS_DRILL,

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

	// 座標を取得
	const DirectX::SimpleMath::Vector3& GetTrans();
	// 座標を設定
	void SetTrans(const DirectX::SimpleMath::Vector3& trans);

	// 弾丸の当たり判定球を取得
	const SphereNode& GetCollisionNodeBody() { return m_CollisionNodeBody; }

	// 死亡フラグを立てる
	void SetDeath()	{ m_Death = true; }
	bool GetDeath() { return m_Death; }

	unsigned int GetSerial() { return m_Serial; }

protected:
	// メンバ変数
	// シリアルナンバー
	unsigned int m_Serial;
	// ３Ｄオブジェクト
	std::vector<Obj3D>	m_Obj;

	Obj3D m_ObjShadow;

	SphereNode m_CollisionNodeBody;

	// 角度
	float m_angle;
	// タイマー
	int m_Timer;

	// ギミックの稼動サイクル
	float m_cycle;
	// 死亡フラグ
	bool m_Death;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	DirectX::SimpleMath::Vector2 m_screenPos;
	DirectX::SimpleMath::Vector2 m_origin;
	bool m_InScreen;
};
