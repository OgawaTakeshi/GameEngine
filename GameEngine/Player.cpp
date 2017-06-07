//--------------------------------------------------------------------------------------
// ファイル名: Player.cpp
// 作成者:
// 作成日:
// 説明:
//--------------------------------------------------------------------------------------

#include "Player.h"
#include <fstream>
#include <sstream>

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

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
	Load();

	m_FireFlag = false;

	m_CollisionNodeBody.Initialize();
	m_CollisionNodeBody.SetParent(&m_Obj[PARTS_TANK]);
	m_CollisionNodeBody.SetLocalRadius(1.0f);
	m_CollisionNodeBody.SetTrans(Vector3(0, 1.0f, 0));

	// 初期化
	m_CollisionNodeBullet.Initialize();
	// 親行列を設定
	m_CollisionNodeBullet.SetParent(&m_Obj[PARTS_GUN_R]);
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
	if (keystate.IsKeyDown(Keyboard::Keys::A))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}

	// 前進/後退
	if (keystate.IsKeyDown(Keyboard::Keys::W))
	{
		// 現在の座標・回転角を取得
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// 移動ベクトル(Z座標前進)
		SimpleMath::Vector3 moveV(0, 0, -0.1f);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		// 移動ベクトルを回転する
		moveV = Vector3::TransformNormal(moveV, rotm);
		// 移動
		trans += moveV;
		// 移動した座標を反映
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::S))
	{
		// 現在の座標・回転角を取得
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// 移動ベクトル(Z座標後退)
		Vector3 moveV(0, 0, +0.1f);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		// 移動ベクトルを回転する
		moveV = Vector3::TransformNormal(moveV, rotm);
		// 移動
		trans += moveV;
		// 移動した座標を反映
		m_Obj[PARTS_TANK].SetTrans(trans);
	}

	// 上昇/下降
	//if (m_pInputManager->GetKeyboard()->IsKeyDown(VK_UP))
	//{
	//	// 現在の座標・回転角を取得
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
	//	// 移動ベクトル(上昇)
	//	SimpleMath::Vector3 moveV(0, 0.1f, 0);
	//	trans += moveV;
	//	// 移動した座標を反映
	//	m_Obj[PARTS_TANK].SetTrans(trans);
	//}
	//if (m_pInputManager->GetKeyboard()->IsKeyDown(VK_DOWN))
	//{
	//	// 現在の座標・回転角を取得
	//	Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
	//	// 移動ベクトル(Z座標後退)
	//	Vector3 moveV(0, -0.1f, 0);
	//	// 移動
	//	trans += moveV;
	//	// 移動した座標を反映
	//	m_Obj[PARTS_TANK].SetTrans(trans);
	//}

	// 腰回転
	if (keystate.IsKeyDown(Keyboard::Keys::Z))
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::X))
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}

	// 胸回転
	if (keystate.IsKeyDown(Keyboard::Keys::C))
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::V))
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}

	// 頭左右回転
	if (keystate.IsKeyDown(Keyboard::Keys::U))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::I))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}

	// 頭上下回転
	if (keystate.IsKeyDown(Keyboard::Keys::O))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.x -= 0.03f;
		if (rot.x < -0.1f)
		{
			rot.x = -0.1f;
		}
		m_Obj[PARTS_HEAD].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::P))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.x += 0.03f;
		if (rot.x > 0.5f)
		{
			rot.x = 0.5f;
		}
		m_Obj[PARTS_HEAD].SetRot(rot);
	}

	// 腕左右回転
	if (keystate.IsKeyDown(Keyboard::Keys::D7))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.y += 0.03f;
		if (rot.y > 0.0f)
		{
			rot.y = 0.0f;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D8))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.y -= 0.03f;
		if (rot.y < -XM_PIDIV2)
		{
			rot.y = -XM_PIDIV2;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}

	// 腕上下回転
	if (keystate.IsKeyDown(Keyboard::Keys::D9))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.x -= 0.03f;
		if (rot.x < -XM_PIDIV4)
		{
			rot.x = -XM_PIDIV4;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::D0))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.x += 0.03f;
		if (rot.x > XM_PIDIV4)
		{
			rot.x = XM_PIDIV4;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}

	// 銃左右回転
	if (keystate.IsKeyDown(Keyboard::Keys::J))
	{
		Vector3 rot = m_Obj[PARTS_GUN_R].GetRot();
		rot.y += 0.03f;
		if (rot.y > XM_PIDIV4)
		{
			rot.y = XM_PIDIV4;
		}
		m_Obj[PARTS_GUN_R].SetRot(rot);
	}
	if (keystate.IsKeyDown(Keyboard::Keys::K))
	{
		Vector3 rot = m_Obj[PARTS_GUN_R].GetRot();
		rot.y -= 0.03f;
		if (rot.y < -XM_PIDIV2)
		{
			rot.y = -XM_PIDIV2;
		}
		m_Obj[PARTS_GUN_R].SetRot(rot);
	}

	// メンバ変数でサイン用の角度を加算し続ける
	m_cycle += 0.1f;
	// 1 + (-1~+1)の範囲　つまり(0~2)倍のスケーリングで変動
	float scale = 1.0f + sinf(m_cycle);
	// パーツにスケーリングの数値を設定する
	m_Obj[PARTS_HEAD].SetScale(Vector3(scale));

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::Keys::E))
	{
		if (m_FireFlag)
		{
			ResetBullet();
		}
		else
		{
			// 発射
			FireBullet();
		}
	}

	if (m_FireFlag)
	{
		// 現在の座標を取得
		Vector3 trans = m_Obj[PARTS_GUN_R].GetTrans();

		trans += m_BulletVel;
		
		// 移動した座標を反映
		m_Obj[PARTS_GUN_R].SetTrans(trans);

		if (--m_FireCount < 0)
		{
			ResetBullet();
		}
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

void Player::Load()
{
	wifstream ifs("CSV/Player.csv");

	wstring line;

	// 1行目をスキップ
	getline(ifs, line);

	vector<wstring> part_names;
	vector<wstring> parent_names;

	// 名簿からデータを解析
	while (getline(ifs, line)) {

		wstring filepath;
		wstring filename;

		// 1行分を文字列ストリームに変換
		wistringstream stream(line);

		// ファイル名の文字列を読み込み
		stream >> filename;
		// プロジェクトの基点からの相対パスでファイル名を補う
		filepath = L"Resources/" + filename + L".cmo";

		Obj3D obj;
		// 読み込む
		obj.LoadModelFile(filepath.c_str());

		// スケーリングの読み取り
		Vector3 scale;
		stream >> scale.x;
		stream >> scale.y;
		stream >> scale.z;
		obj.SetScale(scale);

		// 角度の読み取り
		Vector3 rotation;
		stream >> rotation.z;
		stream >> rotation.x;
		stream >> rotation.y;
		obj.SetRot(rotation);

		// 座標の読み取り
		Vector3 position;
		stream >> position.x;
		stream >> position.y;
		stream >> position.z;
		obj.SetTrans(position);

		m_Obj.push_back(obj);

		wstring parent_name;

		stream >> parent_name;

		part_names.push_back(filename);

		parent_names.push_back(parent_name);
	}

	// 親子関係の組み立て
	for (int i = 0; i < m_Obj.size(); i++)
	{
		// 親の指定あり
		if (parent_names[i].length() > 0)
		{
			for (int j = 0; j < m_Obj.size(); j++)
			{
				if (j == i) continue;

				// 指定の親発見
				if (part_names[j] == parent_names[i])
				{
					m_Obj[i].SetParent(&m_Obj[j]);
				}
			}
		}
	}
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
	m_Obj[PARTS_GUN_R].SetParent(nullptr);
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
	
	m_Obj[PARTS_GUN_R].SetParent(&m_Obj[PARTS_ARM_R]);

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