//--------------------------------------------------------------------------------------
// ファイル名: CollisionNode.cpp
// 作成者:
// 作成日:
// 説明: 衝突判定用ノード
//--------------------------------------------------------------------------------------
#include "CollisionNode.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// デバッグ表示のON/OFFフラグの実体
bool CollisionNode::s_DebugVisible = false;

SphereNode::SphereNode()
{
	// 初期値（半径1m）
	m_LocalRadius = 1.0f;
}

void SphereNode::Initialize()
{
	// デバッグ表示用モデルの読み込み
	// モデルは半径1mで作成すること。
	m_Obj.LoadModelFile(L"Resources/SphereNode.cmo");
}

void SphereNode::Update()
{
	// オブジェクトの行列を更新
	{
		// 親となるオブジェクトからの位置
		m_Obj.SetTrans(m_Trans);
		Vector3 local_scale;
		// 等倍で半径1mの球モデルなので、半径がそのままスケーリングになる
		local_scale.x = m_LocalRadius;
		local_scale.y = m_LocalRadius;
		local_scale.z = m_LocalRadius;
		m_Obj.SetScale(local_scale);
		// 親子関係も含めた行列計算
		m_Obj.Calc();
	}

	// ワールド座標系での当たり判定球を計算
	{
		// 計算済みワールド行列を取得
		Matrix worldm = m_Obj.GetLocalWorld();
		// モデル座標系での中心
		Vector3 center(0, 0, 0);
		// モデル座標系での右端
		Vector3 right(1, 0, 0);

		// ワールド座標に変換
		center = Vector3::Transform(center, worldm);
		right = Vector3::Transform(right, worldm);

		// ワールド座標系での中心座標を設定
		Sphere::center = center;
		// ワールド座標系での半径を計算
		Sphere::radius = Vector3::Distance(center, right);
	}
}

void SphereNode::Draw()
{
	// デバッグ表示がONなら
	if (CollisionNode::GetDebugVisible())
	{
		m_Obj.Draw();
	}
}

void SphereNode::SetParentMatrix(const Matrix* pParentMatrix)
{
	m_Obj.SetParentMatrix(pParentMatrix);
}


//--------------------------------------------------------------
CapsuleNode::CapsuleNode()
{
	// 初期値（半径1m）
	m_LocalRadius = 1.0f;
	// 初期値（軸長さ3m）
	m_LocalLength = 1.0f;
}

void CapsuleNode::Initialize()
{
	// デバッグ表示用モデルの読み込み
	// モデルは半径1m、軸長さ1mの円柱で作成すること。
	m_Obj.LoadModelFile(L"Resources/CapsuleNode.cmo");
}

void CapsuleNode::Update()
{
	// オブジェクトの行列を更新
	{
		// 親となるオブジェクトからの位置
		m_Obj.SetTrans(m_Trans);
		Vector3 local_scale;
		// 等倍で半径1mの円柱モデルなので、半径がそのままスケーリングになる
		local_scale.x = m_LocalRadius;
		local_scale.z = m_LocalRadius;
		// 等倍で軸線長さ1mの円柱モデルなので、長さがそのままスケーリングになる
		local_scale.y = m_LocalLength;
		m_Obj.SetScale(local_scale);
		// 親子関係も含めた行列計算
		m_Obj.Calc();
	}

	// ワールド座標系での当たり判定カプセルを計算
	{
		// 計算済みワールド行列を取得
		Matrix worldm = m_Obj.GetLocalWorld();	
		// モデル座標系での中心
		Vector3 center(0, 0, 0);
		// モデル座標系での上端
		Vector3 up(0, 1, 0);
		// モデル座標系での右端
		Vector3 right(1, 0, 0);

		// ワールド座標に変換
		center = Vector3::Transform(center, worldm);
		up = Vector3::Transform(up, worldm);
		right = Vector3::Transform(right, worldm);

		// ワールド座標系での軸線座標を設定
		Capsule::segment.start = center;
		Capsule::segment.end = up;
		// ワールド座標系での半径を計算
		Capsule::radius = Vector3::Distance(center, right);
	}
}

void CapsuleNode::Draw()
{
	// デバッグ表示がONなら
	if (CollisionNode::GetDebugVisible())
	{
		m_Obj.Draw();
	}
}

void CapsuleNode::SetParentMatrix(const Matrix* pParentMatrix)
{
	m_Obj.SetParentMatrix(pParentMatrix);
}

void CapsuleNode::SetRot(const Vector3& rot)
{
	m_Obj.SetRot(rot);
}