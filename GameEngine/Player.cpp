//--------------------------------------------------------------------------------------
// ファイル名: Player.cpp
// 作成者:
// 作成日:
// 説明:
//--------------------------------------------------------------------------------------

#include "Player.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "DeviceResources.h"
#include "LockOnCamera.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace std;

const float Player::GRAVITY_ACC = 0.03f;
const float Player::JUMP_SPEED_FIRST = 0.5f;
const float Player::JUMP_SPEED_MAX = 0.3f;

Player::Player()
: m_cycle(0.0f)
{
	m_pLandShapeArray = nullptr;

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

	//SetTrans(Vector3(0, 0, 7));

	m_CollisionNodeBody.Initialize();
	m_CollisionNodeBody.SetParent(&m_Obj[PARTS_TANK]);
	m_CollisionNodeBody.SetLocalRadius(1.0f);
	m_CollisionNodeBody.SetTrans(Vector3(0, 1.0f, 0));

	// 初期化
	m_CollisionNodeBullet.Initialize();
	// 親行列を設定
	m_CollisionNodeBullet.SetParent(&m_Obj[PARTS_GUN_R]);
	// 半径
	m_CollisionNodeBullet.SetLocalRadius(0.3f);
	// 親行列からの位置
	m_CollisionNodeBullet.SetTrans(Vector3(0, 0, 0));

	m_isJump = false;

	m_ObjShadow.LoadModel(L"shadow");
	m_ObjShadow.SetTrans(Vector3(0, SHADOW_OFFSET, 0));
}

/// <summary>
/// 通常カメラでの操作
/// </summary>
void Player::ControlNormal()
{
	KeyboardUtil* key = DX::DeviceResources::GetInstance()->GetKeyboardUtil();

	// 前進/後退
	if (key->IsPressed(Keyboard::Keys::W))
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
	if (key->IsPressed(Keyboard::Keys::S))
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

	// １フレームでの旋回速度<ラジアン>
	const float ROT_SPEED = 0.03f;

	// ←回転/→回転
	if (key->IsPressed(Keyboard::Keys::A))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y += ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::D))
	{
		// 現在の角度を取得
		Vector3 rot = m_Obj[PARTS_TANK].GetRot();
		rot.y -= ROT_SPEED;
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(rot);
	}
}

/// <summary>
/// ロックオン時操作
/// </summary>
void Player::ControlLockOn()
{
	LockOn* lockOn = Game::GetInstance()->GetLockOn();
	KeyboardUtil* key = DX::DeviceResources::GetInstance()->GetKeyboardUtil();

	assert(lockOn);
	assert(lockOn->IsLockOn());

	Enemy* enemy = lockOn->SearchLockingEnemy();
	if (enemy == nullptr) return;

	// ロックオン対象方向を向く
	{
		Vector3 player2enemy = enemy->GetTrans() - this->GetTrans();

		float rotY = atan2f(-player2enemy.x, -player2enemy.z);
		// 回転後の角度を反映
		m_Obj[PARTS_TANK].SetRot(Vector3(0, rotY, 0));
	}

	// 前進/後退
	if (key->IsPressed(Keyboard::Keys::W))
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
	if (key->IsPressed(Keyboard::Keys::S))
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

	// 左右移動
	if (key->IsPressed(Keyboard::Keys::A))
	{
		// 現在の座標・回転角を取得
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// 移動ベクトル(Z座標前進)
		SimpleMath::Vector3 moveV(-0.1f, 0, 0);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		// 移動ベクトルを回転する
		moveV = Vector3::TransformNormal(moveV, rotm);
		// 移動
		trans += moveV;
		// 移動した座標を反映
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
	if (key->IsPressed(Keyboard::Keys::D))
	{
		// 現在の座標・回転角を取得
		Vector3 trans = m_Obj[PARTS_TANK].GetTrans();
		float rot_y = m_Obj[PARTS_TANK].GetRot().y;
		// 移動ベクトル(Z座標前進)
		SimpleMath::Vector3 moveV(0.1f, 0, 0);
		Matrix rotm = Matrix::CreateRotationY(rot_y);
		// 移動ベクトルを回転する
		moveV = Vector3::TransformNormal(moveV, rotm);
		// 移動
		trans += moveV;
		// 移動した座標を反映
		m_Obj[PARTS_TANK].SetTrans(trans);
	}
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Player::Update()
{
	KeyboardUtil* key = DX::DeviceResources::GetInstance()->GetKeyboardUtil();

	// ロックオン切り替えボタン
	if (key->IsTriggered(Keyboard::Keys::Enter))
	{
		LockOn* lockOn = Game::GetInstance()->GetLockOn();
		if (lockOn->IsLockOn())
		{
			// ロックオン終了
			lockOn->End();
		}
		else
		{
			// ロックオン開始
			lockOn->Start();
		}
	}

	if (key->IsTriggered(Keyboard::Keys::Space))
	{
		StartJump();
	}
	if (m_isJump)
	{
		m_Velocity.y -= GRAVITY_ACC;
		if (m_Velocity.y < -JUMP_SPEED_MAX)
		{
			m_Velocity.y = -JUMP_SPEED_MAX;
		}
	}

	Vector3 pos = m_Obj[PARTS_TANK].GetTrans();
	pos += m_Velocity;
	m_Obj[PARTS_TANK].SetTrans(pos);

	LockOn* lockOn = Game::GetInstance()->GetLockOn();
	if (lockOn->IsLockOn())
	{
		ControlLockOn();
	}
	else
	{
		ControlNormal();
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
	if (key->IsPressed(Keyboard::Keys::Z))
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::X))
	{
		Vector3 rot = m_Obj[PARTS_WAIST].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_WAIST].SetRot(rot);
	}

	// 胸回転
	if (key->IsPressed(Keyboard::Keys::C))
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::V))
	{
		Vector3 rot = m_Obj[PARTS_BREAST].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_BREAST].SetRot(rot);
	}

	// 頭左右回転
	if (key->IsPressed(Keyboard::Keys::U))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y += 0.03f;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::I))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.y -= 0.03f;
		m_Obj[PARTS_HEAD].SetRot(rot);
	}

	// 頭上下回転
	if (key->IsPressed(Keyboard::Keys::O))
	{
		Vector3 rot = m_Obj[PARTS_HEAD].GetRot();
		rot.x -= 0.03f;
		if (rot.x < -0.1f)
		{
			rot.x = -0.1f;
		}
		m_Obj[PARTS_HEAD].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::P))
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
	if (key->IsPressed(Keyboard::Keys::D7))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.y += 0.03f;
		if (rot.y > 0.0f)
		{
			rot.y = 0.0f;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::D8))
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
	if (key->IsPressed(Keyboard::Keys::D9))
	{
		Vector3 rot = m_Obj[PARTS_ARM_R].GetRot();
		rot.x -= 0.03f;
		if (rot.x < -XM_PIDIV4)
		{
			rot.x = -XM_PIDIV4;
		}
		m_Obj[PARTS_ARM_R].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::D0))
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
	if (key->IsPressed(Keyboard::Keys::J))
	{
		Vector3 rot = m_Obj[PARTS_GUN_R].GetRot();
		rot.y += 0.03f;
		if (rot.y > XM_PIDIV4)
		{
			rot.y = XM_PIDIV4;
		}
		m_Obj[PARTS_GUN_R].SetRot(rot);
	}
	if (key->IsPressed(Keyboard::Keys::K))
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

	if (key->IsTriggered(Keyboard::Keys::R))
	{
		FireHomingBullets();
	}

	if (key->IsTriggered(Keyboard::Keys::E))
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
	{
		Sphere sphere = GetCollisionNodeBody();
		Vector3 trans = GetTrans();
		Vector3 sphere2player = trans - sphere.center;

		const int REPEAT_LIMIT = 5;

		int rejectNum = 0;

		for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray->begin();
			it != m_pLandShapeArray->end();
			)
		{
			LandShape* pLandShape = it->get();

			Vector3 reject;	// 排斥ベクトルを入れるための変数

			if (pLandShape->IntersectSphere(sphere, &reject))
			{
				// めり込み分だけ、球を押し出すように移動
				sphere.center = sphere.center + reject;
				if (++rejectNum >= REPEAT_LIMIT)
				{
					break;
				}
				it = m_pLandShapeArray->begin();
			}
			else
			{
				it++;
			}
		}

		SetTrans(sphere.center + sphere2player);

		Calc();
	}

	// 影の更新
	{
		Vector3 pos = m_Obj[0].GetTrans();
		pos.y = SHADOW_OFFSET;
		m_ObjShadow.SetTrans(pos);
	}

	// 垂直方向地形あたり判定
	{
		const Vector3& vel = GetVelocity();
		
		bool hit = false;
		Segment player_segment;
		Vector3 player_pos = GetTrans();
		player_segment.start = player_pos + Vector3(0, SEGMENT_LENGTH, 0);
		// 影の位置を検出するため、足元から下方向に余裕をもって判定を取る
		player_segment.end = player_pos + Vector3(0, -SHADOW_DISTANCE, 0);

		// 大きい数字で初期化
		float distance = 1.0e5;
		Vector3 inter;

		for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray->begin();
			it != m_pLandShapeArray->end();
			it++)
		{
			LandShape* pLandShape = it->get();
			float temp_distance;
			Vector3 temp_inter;

			// 床面との当たりを判定
			if (pLandShape->IntersectSegmentFloor(player_segment, &temp_inter))
			{
				hit = true;
				temp_distance = Vector3::Distance(player_segment.start, temp_inter);
				if (temp_distance < distance)
				{
					inter = temp_inter;
					distance = temp_distance;
				}
			}
		}

		bool landing = false;
		// ヒット
		if (hit)
		{
			// 速度が上向きでない？
			if (vel.y <= 0.0f)
			{
				// 吸着距離の範囲内か？
				if (distance <= SEGMENT_LENGTH + ADSORP_LENGTH)
				{
					// 着地
					landing = true;
					Vector3 new_position = player_pos;
					new_position.y = inter.y;
					StopJump();
					SetTrans(new_position);
				}
			}
				
			// 影の位置更新
			Vector3 shadow_pos = player_pos;
			shadow_pos.y = inter.y + SHADOW_OFFSET;
			m_ObjShadow.SetTrans(shadow_pos);
		}

		// 着地でなければ
		if (!landing)
		{// 落下開始
			StartFall();
		}

		Calc();
	}

}

//-----------------------------------------------------------------------------
// 行列更新
//-----------------------------------------------------------------------------
void Player::Calc()
{
	// 全パーツ分行列更新
	for (int i = 0; i < PARTS_NUM; i++)
	{
		m_Obj[i].Update();
	}

	// 影の更新
	m_ObjShadow.Update();

	m_CollisionNodeBody.Update();
	// 当たり判定の更新（親の行列更新後に行うこと）
	m_CollisionNodeBullet.Update();
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Player::Draw()
{
	// 影を減算描画
	m_ObjShadow.DrawSubtractive();

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

		wchar_t del = L',';

		// カンマを空白に置き換え
		std::replace(line.begin(), line.end(), L',', L' ');

		// 1行分を文字列ストリームに変換
		wistringstream stream(line);

		// ファイル名の文字列を読み込み
		stream >> filename;

		Obj3D obj;
		// 読み込む
		obj.LoadModel(filename.c_str());

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
	for (unsigned int i = 0; i < m_Obj.size(); i++)
	{
		// 親の指定あり
		if (parent_names[i].length() > 0)
		{
			for (unsigned int j = 0; j < m_Obj.size(); j++)
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

	const float BULLET_SPEED = 0.5f;

	// 発射する弾丸の速度ベクトル
	LockOn* lockOn = Game::GetInstance()->GetLockOn();
	if (lockOn->IsLockOn())
	{
		Enemy* enemy = lockOn->SearchLockingEnemy();

		if (enemy == nullptr) return;

		m_BulletVel = enemy->GetTrans() - pos;
		m_BulletVel.Normalize();
		m_BulletVel *= BULLET_SPEED;
	}
	else
	{
		m_BulletVel = Vector3(0, 0.0f, -BULLET_SPEED);
		// ベクトルをクォータニオンで回転
		m_BulletVel = Vector3::Transform(m_BulletVel, rotq);
	}

	// 自動的に取り付けなおす為のカウントダウン
	m_FireCount = 120;
	m_FireFlag = true;
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

// ホーミング弾発射
void Player::FireHomingBullets()
{
	Game::GetInstance()->FireHomingBullets(this->GetTrans());
}