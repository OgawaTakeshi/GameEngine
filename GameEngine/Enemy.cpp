//--------------------------------------------------------------------------------------
// ファイル名: Enemy.h
// 作成者:
// 作成日:
// 説明:
//--------------------------------------------------------------------------------------

#include <WICTextureLoader.h>

#include "Enemy.h"
#include "DeviceResources.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

unsigned int Enemy::s_SerialCount = 0;

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
	// 敵共通の通し番号を振る
	m_Serial = s_SerialCount++;

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
	m_Obj[PARTS_BODY].LoadModel(L"body");
	m_Obj[PARTS_COCKPIT].LoadModel(L"cockpit");
	m_Obj[PARTS_LAUNCHER].LoadModel(L"launcher");
	m_Obj[PARTS_SHIELD].LoadModel(L"shield");
	m_Obj[PARTS_DRILL].LoadModel(L"drill");

	// パーツの親子関係をセット
	m_Obj[PARTS_COCKPIT].SetParent(
		&m_Obj[PARTS_BODY]);

	m_Obj[PARTS_DRILL].SetParent(
		&m_Obj[PARTS_COCKPIT]);

	m_Obj[PARTS_LAUNCHER].SetParent(
		&m_Obj[PARTS_BODY]);

	m_Obj[PARTS_SHIELD].SetParent(
		&m_Obj[PARTS_BODY]);

	// 親からのオフセット（座標のずらし分）をセット
	m_Obj[PARTS_COCKPIT].SetTrans(
		Vector3(0, 0.37f, -0.4f));
	m_Obj[PARTS_COCKPIT].SetRot(
		Vector3(0, XM_PI, 0));

	m_Obj[PARTS_DRILL].SetTrans(
		Vector3(0, 0.1f, 0.8f));

	m_Obj[PARTS_LAUNCHER].SetTrans(
		Vector3(0, 0.37f, 0.4f));

	m_Obj[PARTS_SHIELD].SetTrans(
		Vector3(-0.8f, 0.37f, 0));
	m_Obj[PARTS_SHIELD].SetScale(
		Vector3(2, 2, 2));
	m_Obj[PARTS_SHIELD].SetRot(
		Vector3(0, 0, XM_PIDIV2));

	// 初期配置ランダム
	Vector3 pos;
	//pos.x = (float)rand() / RAND_MAX * 20.0f - 10.0f;
	//pos.y = 0.5f;
	//pos.z = (float)rand() / RAND_MAX * 20.0f - 10.0f;
	//m_Obj[0].SetTrans(pos);
	float distance = (float)rand() / RAND_MAX * 100.0f;
	float angle = (float)rand() / RAND_MAX * XM_2PI;
	pos = Vector3::UnitX;
	// ワールド原点からの距離
	pos *= distance;
	// 原点周りに回転した位置に配置
	pos = Vector3::Transform(pos, Matrix::CreateRotationY(angle));
	this->SetTrans(pos);

	m_angle = (float)(rand() % 360);

	m_Obj[0].SetRot(Vector3(0, XMConvertToRadians(m_angle), 0));

	// 当たり判定
	m_CollisionNodeBody.Initialize();
	m_CollisionNodeBody.SetParent(&m_Obj[0]);
	m_CollisionNodeBody.SetTrans(Vector3(0, 0.3f, 0));
	m_CollisionNodeBody.SetLocalRadius(1.0f);

	m_ObjShadow.LoadModel(L"shadow");
	m_ObjShadow.SetTrans(Vector3(0, -0.4f, 0));
	m_ObjShadow.SetParent(&m_Obj[0]);

	Microsoft::WRL::ComPtr<ID3D11Resource> resTexture;

	// テクスチャのロード
	CreateWICTextureFromFile(DX::DeviceResources::GetInstance()->GetD3DDevice(), L"PNG/mark_kiduna.png", resTexture.GetAddressOf(),
		m_texture.ReleaseAndGetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	resTexture.As(&texture);

	// テクスチャの中心を割り出す
	CD3D11_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);

	m_origin.x = float(textureDesc.Width / 2);
	m_origin.y = float(textureDesc.Height / 2);

	m_InScreen = false;
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
		Vector3 rotv = m_Obj[0].GetRot();
		float angle = GetShortAngleRad(rotv.y, XMConvertToRadians(m_angle));
		rotv.y += angle*0.01f;
		m_Obj[0].SetRot(rotv);
	}

	// 機体の向いている方向に進む
	{
		// 今の座標を取得
		Vector3 trans = m_Obj[0].GetTrans();

		Vector3 move(0, 0, -0.02f);
		Vector3 rotv = m_Obj[0].GetRot();
		Matrix rotm = Matrix::CreateRotationY(rotv.y);
		move = Vector3::TransformNormal(move, rotm);
		// 座標を移動
		trans += move;
		// 移動後の座標をセット
		m_Obj[0].SetTrans(trans);
	}

	// 移動を反映して行列更新
	Calc();

	m_CollisionNodeBody.Update();
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
		m_Obj[i].Update();
	}
	
	m_ObjShadow.Update();

	m_InScreen = false;
	// 敵のワールド座標に対応するスクリーン座標を得る
	Vector2 screenPos;
	if(Obj3D::GetCamera()->Project(m_Obj[0].GetTrans(), &screenPos))
	{
		// ビューポートの取得
		D3D11_VIEWPORT viewport = DX::DeviceResources::GetInstance()->GetScreenViewport();

		// 画面の範囲に収まっているか？
		if (viewport.TopLeftX <= screenPos.x && screenPos.x <= viewport.TopLeftX + viewport.Width &&
			viewport.TopLeftY <= screenPos.y && screenPos.y <= viewport.TopLeftY + viewport.Height)
		{
			// スクリーン座標を記録
			m_screenPos = screenPos;
			m_InScreen = true;
		}
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

	m_CollisionNodeBody.Draw();

	// 影を減算描画
	m_ObjShadow.DrawSubtractive();

	//// アイコン描画
	//if (m_InScreen)
	//{
	//	DX::DeviceResources::GetInstance()->GetSpriteBatch()->Draw(m_texture.Get(), m_screenPos, nullptr, Colors::White,
	//		0.f, m_origin, 0.2f);
	//}
}

const DirectX::SimpleMath::Vector3 & Enemy::GetTrans()
{
	// ベースパーツの座標を返す
	return m_Obj[0].GetTrans();
}

void Enemy::SetTrans(const DirectX::SimpleMath::Vector3 & trans)
{
	// ベースパーツの座標を設定
	m_Obj[0].SetTrans(trans);
}
