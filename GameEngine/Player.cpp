//--------------------------------------------------------------------------------------
// ファイル名: Player.cpp
// 作成者:
// 作成日:
// 説明:
//--------------------------------------------------------------------------------------
#include "Player.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// static変数の実体
const float Player::GRAVITY_ACC = 0.03f;
const float Player::JUMP_SPEED_FIRST = 0.5f;
const float Player::JUMP_SPEED_MAX = 0.3f;


Player::Player(DirectX::Keyboard* keyboard)
{
	m_pKeyboard = keyboard;

	Initialize();
}

Player::~Player()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Player::Initialize()
{
	m_Obj.resize(PARTS_NUM);
	// ロボットのモデル読み込み
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

	// 当たり判定ノードの設定
	{
		// 初期化
		m_CollisionNodeBody.Initialize();
		// 親行列を設定
		m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
		// 球の半径
		m_CollisionNodeBody.SetLocalRadius(0.7f);
		// 親行列からの位置
		m_CollisionNodeBody.SetTrans(Vector3(0, 0.7f, 0));
	}

}
//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Player::Update()
{
	Keyboard::State keystate = m_pKeyboard->GetState();

	// １フレームでの旋回速度<ラジアン>
	const float ROT_SPEED = 0.03f;
	// ←回転/→回転
	if (keystate.A)
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.D)
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	//if (g_key.Up)
	//{
	//	// 現在の角度を取得
	//	Vector3 rot = m_Obj[PARTS_TANK].GetRot();
	//	rot.x += ROT_SPEED;
	//	// 回転後の角度を反映
	//	m_Obj[PARTS_TANK].SetRot(rot);
	//}
	//if (g_key.Down)
	//{
	//	// 現在の角度を取得
	//	Vector3 rot = m_Obj[PARTS_TANK].GetRot();
	//	rot.x -= ROT_SPEED;
	//	// 回転後の角度を反映
	//	m_Obj[PARTS_TANK].SetRot(rot);
	//}
	// 前進/後退
	if (keystate.W)
	{
		// 現在の座標・回転角を取得
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		//float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// 移動ベクトル(Z座標前進)
		//SimpleMath::Vector3 moveV(0, 0, -0.1f);
		SimpleMath::Vector3 moveV(0, 0,-0.1f);
		//Matrix rotm = Matrix::CreateRotationY(rot_y);
		//Matrix rotm = Matrix::CreateFromYawPitchRoll(rot.y, rot.x, rot.z);
		const Matrix& rotm = GetLocalWorld();
		// 移動ベクトルを回転する
		moveV = Vector3::TransformNormal(moveV, rotm);
		// 移動
		trans += moveV;
		// 移動した座標を反映
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	if (keystate.S)
	{
		// 現在の座標・回転角を取得
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// 移動ベクトル(Z座標後退)
		//Vector3 moveV(0, 0, +0.1f);
		SimpleMath::Vector3 moveV(0, 0,0.1f);
		//Matrix rotm = Matrix::CreateRotationY(rot_y);
		const Matrix& rotm = GetLocalWorld();
		// 移動ベクトルを回転する
		moveV = Vector3::TransformNormal(moveV, rotm);
		// 移動
		trans += moveV;
		// 移動した座標を反映
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	//// 腰回転
	//..省略..
	//	// 胸回転
	//	..省略..
	//	// 頭左右回転
	//	..省略..
	//	// 頭上下回転
	//	..省略..
	//	// 腕左右回転
	//	..省略..
	//	// 腕上下回転
	//	..省略..
	//	// 銃左右回転
	//	..省略..

	//if (m_isJump)
	//{
	//	m_Velocity += Vector3(0, -0.01, 0);

	//	// 現在の座標・回転角を取得
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();

	//	trans += m_Velocity;

	//	m_Obj[PARTS_TANK].SetTrans(trans);
	//}

	// 移動を反映して行列更新
	Calc();
}
//-----------------------------------------------------------------------------
// 行列更新
//-----------------------------------------------------------------------------
void Player::Calc()
{
	// 全パーツ分行列更新
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Calc();
	}

	// 当たり判定の更新（親の行列更新後に行うこと）
	m_CollisionNodeBody.Update();
}
//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Player::Draw()
{
	// 全パーツ分描画
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Draw();
	}

	// 当たり判定のデバッグ表示
	m_CollisionNodeBody.Draw();

}
const DirectX::SimpleMath::Vector3& Player::GetTrans()
{
	// タンクパーツの座標を返す
	return m_Obj[PARTS_TANK].GetTrans();
}
void Player::SetTrans(const DirectX::SimpleMath::Vector3& trans)
{
	// タンクパーツの座標を設定
	m_Obj[PARTS_TANK].SetTrans(trans);
}
const DirectX::SimpleMath::Matrix& Player::GetLocalWorld()
{
	// タンクパーツのワールド行列を返す
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

// ジャンプを開始する
void Player::StartJump() {
	// ジャンプ中でないか
	if (!m_isJump)
	{
		// 上方向の初速を設定
		m_Velocity.y = JUMP_SPEED_FIRST;
		// ジャンプフラグを立てる
		m_isJump = true;
	}
}

// 落下を開始する
void Player::StartFall() {
	// ジャンプ中でないか
	if (!m_isJump)
	{
		// 上方向の初速は0
		m_Velocity.y = 0.0f;
		// ジャンプフラグを立てる
		m_isJump = true;
	}
}

// ジャンプを終了する
void Player::StopJump() {
	// ジャンプフラグを下ろす
	m_isJump = false;
	// デフォルト値で初期化
	m_Velocity = Vector3();
}
