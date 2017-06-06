//--------------------------------------------------------------------------------------
// ファイル名: Enemy.h
// 作成者:
// 作成日:
// 説明:
//--------------------------------------------------------------------------------------

#include "Enemy.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/*==================================
目的の角度への最短角度を取得（ラジアン）

引数	_angle0	ベースとなる角度
_angle1	目標とする角度
戻り値	差分角度

角度０から角度１に最短コースで向かう際に加算する角度を取得する
===================================*/
static float GetShortAngleRad(float _angle0, float _angle1)
{
	float angle_sub;

	angle_sub = _angle1 - _angle0;	// 角度の差
	// 差が１８０度(π）より大きかった場合、逆回転の方が近いので、マイナスに変換
	// 最終的に-180～+180度の範囲に。
	if (angle_sub > XM_PI)
	{
		angle_sub -= XM_2PI;
	}
	else if (angle_sub < -XM_PI)
	{
		angle_sub += XM_2PI;
	}

	return angle_sub;
}

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Enemy::Enemy()
: m_cycle(0.0f)
, m_Death(false)
{
	m_Timer = 60;
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Enemy::~Enemy()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Enemy::Initialize()
{
	m_Obj.resize(PARTS_NUM);
	// モデル読み込み
	m_Obj[PARTS_TANK].LoadModelFile(L"Resources/tank.cmo");
	m_Obj[PARTS_WAIST].LoadModelFile(L"Resources/waist.cmo");
	m_Obj[PARTS_BREAST].LoadModelFile(L"Resources/breast.cmo");
	m_Obj[PARTS_HEAD].LoadModelFile(L"Resources/head.cmo");
	m_Obj[PARTS_ARM_R].LoadModelFile(L"Resources/arm.cmo");
	m_Obj[PARTS_GUN_R].LoadModelFile(L"Resources/gun.cmo");

	// 親子関係の組み立て
	m_Obj[PARTS_WAIST].SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	m_Obj[PARTS_BREAST].SetParentMatrix(&m_Obj[PARTS_WAIST].GetLocalWorld());
	m_Obj[PARTS_HEAD].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());
	m_Obj[PARTS_ARM_R].SetParentMatrix(&m_Obj[PARTS_BREAST].GetLocalWorld());
	m_Obj[PARTS_GUN_R].SetParentMatrix(&m_Obj[PARTS_ARM_R].GetLocalWorld());

	// 親からのオフセット（位置の差）を設定
	m_Obj[PARTS_TANK].SetTrans(Vector3(0, 0.01f, 0));
	m_Obj[PARTS_WAIST].SetScale(Vector3(0.7f, 0.7f, 0.7f));
	m_Obj[PARTS_WAIST].SetTrans(Vector3(0, 0.40f, 0));
	m_Obj[PARTS_BREAST].SetTrans(Vector3(0, 0.5f, 0));
	m_Obj[PARTS_HEAD].SetTrans(Vector3(0, 0.42f, 0));
	m_Obj[PARTS_ARM_R].SetTrans(Vector3(0.5f, 0.3f, 0));
	m_Obj[PARTS_GUN_R].SetTrans(Vector3(0.2f, 0.45f, -0.3f));

	Vector3 pos;
	pos.x = (float)rand() / RAND_MAX * 20.0f - 10.0f;
	pos.z = (float)rand() / RAND_MAX * 20.0f - 10.0f;
	m_Obj[PARTS_TANK].SetTrans(pos);

	m_angle = (float)(rand() % 360);

	m_Obj[PARTS_TANK].SetRot(Vector3(0, XMConvertToRadians(m_angle), 0));

	//m_CollisionNodeBody.Initialize();
	//m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	//m_CollisionNodeBody.SetTrans(Vector3(0, 0.7f, 0));
	//m_CollisionNodeBody.SetLocalRadius(0.7f);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Enemy::Update()
{
	// 死んでいたら何もしない
	if (m_Death) return;

	// 定期的に進行方向を変える
	m_Timer--;
	if (m_Timer < 0)
	{
		m_Timer = 60;

		// 角度を変更
		float rnd = (float)rand() / RAND_MAX - 0.5f;
		rnd *= 180.0f;
		m_angle += rnd;
	}

	// じわじわと角度を反映
	{
		Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
		float angle = GetShortAngleRad(rotv.y, XMConvertToRadians(m_angle));
		rotv.y += angle*0.01f;
		m_Obj[PARTS_TANK].SetRot(rotv);
	}

	// 機体の向いている方向に進む
	{
		// 今の座標を取得
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();

		Vector3 move(0, 0, -0.02f);
		Vector3 rotv = m_Obj[PARTS_TANK].GetRot();
		Matrix rotm = Matrix::CreateRotationY(rotv.y);
		move = Vector3::TransformNormal(move, rotm);
		// 座標を移動
		trans += move;
		// 移動後の座標をセット
		m_Obj[PARTS_TANK].SetTrans(trans);
	}

	// 移動を反映して行列更新
	Calc();

	//m_CollisionNodeBody.Update();
}

//-----------------------------------------------------------------------------
// 行列更新
//-----------------------------------------------------------------------------
void Enemy::Calc()
{
	// 死んでいたら何もしない
	if (m_Death) return;

	// 全パーツ分行列更新
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Calc();
	}
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Enemy::Draw()
{
	// 死んでいたら何もしない
	if (m_Death) return;

	// 全パーツ分描画
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Draw();
	}

	//m_CollisionNodeBody.Draw();
}
