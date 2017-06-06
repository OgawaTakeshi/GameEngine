//--------------------------------------------------------------------------------------
// ファイル名: Player.cpp
// 作成者:
// 作成日:
// 説明:
//--------------------------------------------------------------------------------------

#include "Player.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

const float Player::GRAVITY_ACC = 0.03f;
const float Player::JUMP_SPEED_FIRST = 0.5f;
const float Player::JUMP_SPEED_MAX = 0.3f;

Player::Player(DirectX::Keyboard* keyboard)
: m_cycle(0.0f)
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

	m_FireFlag = false;

	m_CollisionNodeBody.Initialize();
	m_CollisionNodeBody.SetParentMatrix(&m_Obj[PARTS_TANK].GetLocalWorld());
	m_CollisionNodeBody.SetLocalRadius(1.0f);
	m_CollisionNodeBody.SetTrans(Vector3(0, 1.0f, 0));

	// 初期化
	m_CollisionNodeBullet.Initialize();
	// 親行列を設定
	m_CollisionNodeBullet.SetParentMatrix(&m_Obj[PARTS_GUN_R].GetLocalWorld());
	// カプセルの半径
	m_CollisionNodeBullet.SetLocalRadius(0.3f);
	// カプセルの軸長さ
	m_CollisionNodeBullet.SetLength(1.4f);
	// 親行列からの位置
	m_CollisionNodeBullet.SetTrans(Vector3(0, 0, 0));
	// 親行列からの回転
	m_CollisionNodeBullet.SetRot(Vector3(-XM_PIDIV2, 0, 0));

	m_isJump = false;
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Player::Update()
{
	Keyboard::State keystate = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keystate);

	//if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::Space))
	//{
	//	StartJump();
	//}
	//if (m_isJump)
	//{
	//	m_Velocity.y -= GRAVITY_ACC;
	//	if (m_Velocity.y < -JUMP_SPEED_MAX)
	//	{
	//		m_Velocity.y = -JUMP_SPEED_MAX;
	//	}
	//}

	Vector3 pos = m_Obj[PARTS_TANK].GetTrans();
	pos += m_Velocity;
	m_Obj[PARTS_TANK].SetTrans(pos);

	// １フレームでの旋回速度<ラジアン>
	const float ROT_SPEED = 0.03f;

	

	// ←回転/→回転
	if (keystate.IsKeyDown(Keyboard::Keys::Space))
	{
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(Vector3(0,0,0));
	}

	// ←回転/→回転
	if (keystate.IsKeyDown(Keyboard::Keys::Q))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x -= ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::E))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.x += ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	// ←回転/→回転
	if (keystate.IsKeyDown(Keyboard::Keys::A))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	// ←回転/→回転
	if (keystate.IsKeyDown(Keyboard::Keys::Z))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.z -= ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::C))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.z += ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	{
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();

		Game::m_debugText->AddText(Vector2(20, 20), L"Q E:Rotate X ... %+04d", (int)XMConvertToDegrees(rot.x));
		Game::m_debugText->AddText(Vector2(20, 40), L"A D:Rotate Y ... %+04d", (int)XMConvertToDegrees(rot.y));
		Game::m_debugText->AddText(Vector2(20, 60), L"Z C:Rotate Z ... %+04d", (int)XMConvertToDegrees(rot.z));
		Game::m_debugText->AddText(Vector2(20, 80), L"Space: Reset");
	}

	

	// 移動を反映して行列更新
	Calc();

	// 水平方向あたり判定
	//{
	//	Sphere sphere = GetCollisionNodeBody();
	//	Vector3 trans = GetTrans();
	//	Vector3 sphere2player = trans - sphere.center;

	//	for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray->begin();
	//		it != m_pLandShapeArray->end();
	//		it++)
	//	{
	//		LandShape* pLandShape = it->get();

	//		Vector3 reject;	// 排斥ベクトルを入れるための変数

	//		if (pLandShape->IntersectSphere(sphere, &reject))
	//		{
	//			// めり込み分だけ、球を押し出すように移動
	//			sphere.center = sphere.center + reject;
	//		}
	//	}

	//	SetTrans(sphere.center + sphere2player);

	//	Calc();
	//}

	// 垂直方向地形あたり判定
	//{
	//	const Vector3& vel = GetVelocity();

	//	if (vel.y <= 0.0f)
	//	{
	//		bool hit = false;
	//		Segment player_segment;
	//		Vector3 player_pos = GetTrans();
	//		player_segment.start = player_pos + Vector3(0, 1.0f, 0);
	//		player_segment.end = player_pos + Vector3(0, -0.5f, 0);

	//		// 大きい数字で初期化
	//		float distance = 1.0e5;
	//		Vector3 inter;

	//		for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray->begin();
	//			it != m_pLandShapeArray->end();
	//			it++)
	//		{
	//			LandShape* pLandShape = it->get();
	//			float temp_distance;
	//			Vector3 temp_inter;

	//			// 床面との当たりを判定
	//			if (pLandShape->IntersectSegment(player_segment, &temp_inter))
	//			{
	//				hit = true;
	//				temp_distance = Vector3::Distance(player_segment.start, temp_inter);
	//				if (temp_distance < distance)
	//				{
	//					inter = temp_inter;
	//					distance = temp_distance;
	//				}
	//			}
	//		}

	//		if (hit)
	//		{
	//			Vector3 new_position = player_pos;
	//			new_position.y = inter.y;
	//			StopJump();
	//			SetTrans(new_position);
	//		}

	//		if (!hit)
	//		{// 落下開始
	//			StartFall();
	//		}
	//	}

	//	Calc();
	//}

	
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

	m_CollisionNodeBody.Update();
	// 当たり判定の更新（親の行列更新後に行うこと）
	m_CollisionNodeBullet.Update();
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

	m_CollisionNodeBody.Draw();
	// 当たり判定のデバッグ表示
	m_CollisionNodeBullet.Draw();
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

void Player::SetRot(const DirectX::SimpleMath::Vector3& rot)
{
	// タンクパーツの座標を設定
	m_Obj[PARTS_TANK].SetRot(rot);
}

const DirectX::SimpleMath::Matrix& Player::GetLocalWorld()
{
	// タンクパーツのワールド行列を返す
	return m_Obj[PARTS_TANK].GetLocalWorld();
}

// 弾丸用のパーツを射出する
void Player::FireBullet()
{
	// 既に発射中
	if (m_FireFlag)	return;

	m_FireFlag = true;

	// 親子関係を加味したワールド座標を取得
	Matrix worldm = m_Obj[PARTS_GUN_R].GetLocalWorld();

	Vector3 scale;
	Quaternion rotq;
	Vector3 pos;
	// 行列の１行ずつを、Vector3として扱う
	Vector3* m0 = (Vector3*)&worldm.m[0];
	Vector3* m1 = (Vector3*)&worldm.m[1];
	Vector3* m2 = (Vector3*)&worldm.m[2];
	Vector3* m3 = (Vector3*)&worldm.m[3];
	// ワールド座標を取り出す
	pos = *m3;
	// ワールドスケーリングを取り出す
	scale = Vector3(m0->Length(), m1->Length(), m2->Length());
	// スケーリングを打ち消す※これをやらないと、クォータニオンが正しく取れない
	m0->Normalize();
	m1->Normalize();
	m2->Normalize();
	// ワールド回転を取り出す
	rotq = Quaternion::CreateFromRotationMatrix(worldm);

	// 一発でやってくれる関数
	//worldm.Decompose(scale, rotq, pos);

	// 親子関係を解除する
	m_Obj[PARTS_GUN_R].SetParentMatrix(nullptr);
	m_Obj[PARTS_GUN_R].SetScale(scale);
	m_Obj[PARTS_GUN_R].SetRotQ(rotq);
	m_Obj[PARTS_GUN_R].SetTrans(pos);

	// 発射する弾丸の速度ベクトル
	m_BulletVel = Vector3(0, 0.0f,-0.1f);
	// ベクトルをクォータニオンで回転
	m_BulletVel = Vector3::Transform(m_BulletVel, rotq);

	// 自動的に取り付けなおす為のカウントダウン
	m_FireCount = 120;
}

// 弾丸用のパーツをロボットに取り付けなおす
void Player::ResetBullet()
{
	// 発射中ではない
	if (!m_FireFlag)	return;
	
	m_Obj[PARTS_GUN_R].SetParentMatrix(&m_Obj[PARTS_ARM_R].GetLocalWorld());

	m_Obj[PARTS_GUN_R].SetScale(Vector3(1, 1, 1));
	m_Obj[PARTS_GUN_R].SetRot(Vector3(0, 0, 0));
	m_Obj[PARTS_GUN_R].SetTrans(Vector3(0.2f, 0.45f, -0.3f));

	m_FireFlag = false;
}

// ジャンプを開始する
void Player::StartJump()
{
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
void Player::StartFall()
{
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
void Player::StopJump()
{
	// ジャンプフラグを下ろす
	m_isJump = false;
	// デフォルト値で初期化
	m_Velocity = Vector3();
}