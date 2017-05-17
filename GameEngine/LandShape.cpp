//--------------------------------------------------------------------------------------
// ファイル名: LandShape
// 作成者:
// 作成日:
// 説明:地形当たり
//--------------------------------------------------------------------------------------

#include <fstream>
#include <algorithm>
#include "LandShape.h"
#include "CollisionNode.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


// 静的メンバ変数の実体
std::unique_ptr<LandShapeCommon> LandShape::s_pCommon;
std::map<std::wstring, std::unique_ptr<LandShapeData>> LandShape::s_dataarray;

LandShapeCommon::LandShapeCommon(LandShapeCommonDef def)
{
	// カメラ
	m_pCamera = def.pCamera;
	// 描画ステート
	m_pStates.reset(new CommonStates(def.pDevice));
	// エフェクトファクトリ
	m_pEffectFactory.reset(new EffectFactory(def.pDevice));
	// プリミティブバッチ
	m_pPrimitiveBatch.reset(new PrimitiveBatch<VertexPositionNormal>(def.pDeviceContext));
	// エフェクト
	m_pEffect.reset(new BasicEffect(def.pDevice));
	m_pEffect->SetLightingEnabled(true);
	m_pEffect->SetAmbientLightColor(Vector3(0, 0.0f, 0));
	m_pEffect->SetDiffuseColor(Vector3(1.0f, 1.0f, 1.0f));
	m_pEffect->SetLightEnabled(0, true);
	m_pEffect->SetLightDiffuseColor(0, Vector3(0.2f, 1.0f, 0.2f));
	m_pEffect->SetLightDirection(0, Vector3(1, -0.5f, 2));
	m_pEffect->SetLightEnabled(1, true);
	m_pEffect->SetLightDiffuseColor(1, Vector3(0.5f, 0.2f, 0.3f));
	m_pEffect->SetLightDirection(1, Vector3(-1, -0.5f, -2));

	void const* shaderByteCode;
	size_t byteCodeLength;

	// シェーダーの取得
	m_pEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	// 入力レイアウトの作成
	def.pDevice->CreateInputLayout(VertexPositionNormal::InputElements,
		VertexPositionNormal::InputElementCount,
		shaderByteCode, byteCodeLength,
		&m_pInputLayout);
	// デバイスコンテキスト
	m_pDeviceContext = def.pDeviceContext;
}

LandShapeCommon::~LandShapeCommon()
{
	// 入力レイアウトの解放
	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}
}

/**
*	@brief エフェクトファクトリー生成
*/
void LandShape::InitializeCommon(LandShapeCommonDef def)
{
	// 既に初期化済み
	if (s_pCommon)	return;

	// 共通データ生成
	s_pCommon.reset(new LandShapeCommon(def));
}

//--------------------------------------------------------------------------------------
// コンストラクタ
//--------------------------------------------------------------------------------------
LandShape::LandShape()
	: m_pData(nullptr)
{

}

//--------------------------------------------------------------------------------------
// 初期化
//--------------------------------------------------------------------------------------
void LandShape::Initialize(const wchar_t* filename_mdl, const wchar_t* filename_cmo)
{
	if (filename_mdl)
	{
		std::map<std::wstring, std::unique_ptr<LandShapeData>>::iterator it;
		it = s_dataarray.find(filename_mdl);
		if (s_dataarray.count(filename_mdl) == 0)
		{
			// モデルの読み込み
			s_dataarray[filename_mdl] = LandShapeData::CreateFromMDL(filename_mdl);
		}
		// 地形データをセット
		m_pData = s_dataarray[filename_mdl].get();
	}

	if (filename_cmo)
	{
		// オブジェクト初期化
		m_Obj.LoadModelFile(filename_cmo);
	}
}

//--------------------------------------------------------------------------------------
// ワールド行列の計算
//--------------------------------------------------------------------------------------
void LandShape::Calc()
{
	m_Obj.Calc();
	// 逆行列を計算
	const Matrix& localworld = m_Obj.GetLocalWorld();
	m_WorldLocal = localworld.Invert();
}

//--------------------------------------------------------------------------------------
// 地形データの描画
//--------------------------------------------------------------------------------------
void LandShape::Draw()
{
	if (CollisionNode::GetDebugVisible() == false)
	{
		// モデル描画
		m_Obj.Draw();
	}
	else if (m_pData)
	{
		// デバッグ描画
		const Matrix& view = s_pCommon->m_pCamera->GetViewmat();
		const Matrix& projection = s_pCommon->m_pCamera->GetProjmat();

		// 作成した行列をエフェクトにセット
		s_pCommon->m_pEffect->SetWorld(m_Obj.GetLocalWorld());
		s_pCommon->m_pEffect->SetView(view);
		s_pCommon->m_pEffect->SetProjection(projection);

		// エフェクトの設定（各行列やテクスチャなどを設定している）
		s_pCommon->m_pEffect->Apply(s_pCommon->m_pDeviceContext);

		// 深度ステンシル ステートを設定する
		s_pCommon->m_pDeviceContext->OMSetDepthStencilState(s_pCommon->m_pStates->DepthDefault(), 0);

		// ブレンディング ステートを設定する
		s_pCommon->m_pDeviceContext->OMSetBlendState(s_pCommon->m_pStates->NonPremultiplied(), nullptr, 0xFFFFFFFF);

		// ラスタライザ ステートを設定する 時計回りを非表示
		s_pCommon->m_pDeviceContext->RSSetState(s_pCommon->m_pStates->CullClockwise());

		// サンプラーステートを設定する
		auto samplerState = s_pCommon->m_pStates->PointWrap();
		s_pCommon->m_pDeviceContext->PSSetSamplers(0, 1, &samplerState);

		// 入力レイアウトを設定する
		s_pCommon->m_pDeviceContext->IASetInputLayout(s_pCommon->m_pInputLayout);

		// 描画開始
		s_pCommon->m_pPrimitiveBatch->Begin();

		const uint16_t* pIndex = &m_pData->m_Indices[0];
		int numIndex = m_pData->m_Indices.size();

		const VertexPositionNormal* pVertex = (VertexPositionNormal*)&m_pData->m_Vertices[0];
		int numVertex = m_pData->m_Vertices.size();

		// 三角形プリミティブの描画
		s_pCommon->m_pPrimitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, pIndex, numIndex, pVertex, numVertex);

		// 描画終了
		s_pCommon->m_pPrimitiveBatch->End();
	}
}

//--------------------------------------------------------------------------------------
// 地形と球の交差判定
// down : 下方向ベクトル
// reject : 排斥ベクトル
//--------------------------------------------------------------------------------------
bool LandShape::IntersectSphere(const Sphere& sphere, /*Vector3* down*,*/ Vector3* reject)
{
	if (m_pData == nullptr) return false;

	// 頂点インデックスの数を３で割って、三角形の数を計算
	int nTri = m_pData->m_Indices.size() / 3;
	// ヒットフラグを初期化
	bool hit = false;
	// 大きい数字で初期化
	float distance = 1.0e5;
	Vector3 l_inter;
	Vector3 l_normal;
	Vector3 l_down;
	// スケールを取得
	float scale = GetScale();

	// 球をコピー
	Sphere localsphere = sphere;

	// スケール0の場合、判定しない
	if (scale <= 1.0e-10) return false;

	// 球の中心点をワールド座標からモデル座標系に引き込む
	localsphere.center = Vector3::Transform(sphere.center, m_WorldLocal);
	// 半径をワールドをワールド座標系からモデル座標系に変換
	localsphere.radius = sphere.radius / scale;

	// 下方向ベクトルをワールド座標からモデル座標系に引き込む
	//l_down = Vector3::TransformNormal(*down, m_WorldLocal);
	//l_down.Normalize();

	for (int i = 0; i < nTri; i++)
	{
		// 三角形の各頂点のインデックスを取得
		int index0 = m_pData->m_Indices[i * 3];
		int index1 = m_pData->m_Indices[i * 3 + 1];
		int index2 = m_pData->m_Indices[i * 3 + 2];
		// 各頂点のローカル座標を取得
		Vector3 pos0 = m_pData->m_Vertices[index0].Pos;
		Vector3 pos1 = m_pData->m_Vertices[index1].Pos;
		Vector3 pos2 = m_pData->m_Vertices[index2].Pos;

		Triangle tri;

		// 3点から三角形を構築（TODO:先に計算しておく）
		ComputeTriangle(pos0, pos1, pos2, &tri);

		float temp_distance;
		Vector3 temp_inter;

		//float dt = l_down.Dot(tri.Normal);
		//// ポリゴンの向きが上か下方向から30度内なら、鉛直方向の当たりで処理するので、スキップする
		//float theta = XMConvertToDegrees(acosf(dt));
		//if (theta < 30.0f || 150.0f < theta) continue;

		// 三角形と球の当たり判定
		if (CheckSphere2Triangle(localsphere, tri, &temp_inter))
		{
			hit = true;
			temp_distance = Vector3::Distance(localsphere.center, temp_inter);
			if (temp_distance < distance)
			{
				l_inter = temp_inter;
				l_normal = tri.Normal;
				distance = temp_distance;
			}
		}
	}

	if (hit)
	{
		// 距離をモデル座標系からワールド座標系に変換
		distance *= scale;

		// ワールド行列を取得
		const Matrix& localworld = m_Obj.GetLocalWorld();

		// 排斥ベクトルの計算
		if (reject != nullptr)
		{
			// 地形の法線方向をモデル座標系からワールド座標系に変換
			*reject = Vector3::TransformNormal(l_normal, localworld);
			reject->Normalize();
			// めり込み分の長さに伸ばす
			*reject = (*reject) * (sphere.radius - distance);
		}
	}

	return hit;
}

//--------------------------------------------------------------------------------------
// 地形と線分の交差判定
// segment : 線分
// （出力）inter : 交点（ポリゴンの平面上で、点との再接近点の座標を返す）
//--------------------------------------------------------------------------------------
bool LandShape::IntersectSegment(const Segment& segment, Vector3* inter)
{
	if (m_pData == nullptr) return false;

	// 三角形の数
	int nTri = m_pData->m_Indices.size() / 3;
	// ヒットフラグを初期化
	bool hit = false;
	// 大きい数字で初期化
	float distance = 1.0e5;
	// 角度判定用に地面とみなす角度の限界値<度>
	const float limit_angle = XMConvertToRadians(30.0f);
	Vector3 l_inter;

	// コピー
	Segment localSegment = segment;
	// 線分をワールド座標からモデル座標系に引き込む
	localSegment.start = Vector3::Transform(localSegment.start, m_WorldLocal);
	localSegment.end = Vector3::Transform(localSegment.end, m_WorldLocal);
	// 線分の方向ベクトルを取得
	Vector3 segmentNormal = localSegment.end - localSegment.start;
	segmentNormal.Normalize();

	for (int i = 0; i < nTri; i++)
	{
		// 三角形の各頂点のインデックスを取得
		int index0 = m_pData->m_Indices[i * 3];
		int index1 = m_pData->m_Indices[i * 3 + 1];
		int index2 = m_pData->m_Indices[i * 3 + 2];
		// 各頂点のローカル座標を取得
		Vector3 pos0 = m_pData->m_Vertices[index0].Pos;
		Vector3 pos1 = m_pData->m_Vertices[index1].Pos;
		Vector3 pos2 = m_pData->m_Vertices[index2].Pos;

		Triangle tri;

		// 3点から三角形を構築（TODO:先に計算しておく）
		ComputeTriangle(pos0, pos1, pos2, &tri);

		float temp_distance;
		Vector3 temp_inter;

		// 上方向ベクトルと法線の内積
		float dot = -segmentNormal.Dot(tri.Normal);
		// 上方向との角度差を計算
		float angle = acosf(dot);
		// 上方向との角度が限界角より大きければ、地面とみなさず、スキップ
		if ( angle > limit_angle ) continue;

		// 線分と三角形（ポリゴン）の交差判定
		if (CheckSegment2Triangle(localSegment, tri, &temp_inter))
		{
			hit = true;
			// 衝突点までの距離を算出
			temp_distance = Vector3::Distance(localSegment.start, temp_inter);
			// 距離が一番近いものを記録
			if (temp_distance < distance)
			{
				l_inter = temp_inter;
				distance = temp_distance;
			}
		}
	}

	if (hit && inter != nullptr)
	{
		// 交点をモデル座標系からワールド座標系に変換
		const Matrix& localworld = m_Obj.GetLocalWorld();
		*inter = Vector3::Transform(l_inter, localworld);
	}

	return hit;
}