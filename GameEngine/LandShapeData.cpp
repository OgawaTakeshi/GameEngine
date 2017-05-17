#include <fstream>
#include <algorithm>
#include "LandShapeData.h"
#include "CollisionNode.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

// モデルデータ読み込み
std::unique_ptr<LandShapeData> LandShapeData::CreateFromMDL(const char* meshData)
{
	// 最新の読み込み位置を示すポインタ
	const void* head = meshData;

	std::unique_ptr<LandShapeData> landshape(new LandShapeData());

	// ノード数を読み取る
	const UINT* p_nNode = static_cast<const UINT*>(head);
	// 読み込み位置を進める
	head = p_nNode + 1;
	// ノード数取得
	UINT nNode = *p_nNode;


	// ノード情報を読み取る
	const NodeInfo* nodeInfo_array = static_cast<const NodeInfo*>(head);
	head = nodeInfo_array + nNode;

	int mesh = 0;
	// メッシュの頂点データとインデックスデータを読み込み
	landshape->m_Indices.clear();
	// ノード数分×各ノードの頂点データ数の２次元配列
	vector<vector<VERTEX_LANDSHAPE>>	vertex_array;
	vector<vector<USHORT>>				index_array;
	for (UINT i = 0; i < nNode; i++)
	{
		// ノード情報１個分を取得
		const NodeInfo* nodeInfo = &nodeInfo_array[i];

		vector<VERTEX_LANDSHAPE> va;
		vector<USHORT> ia;

		vertex_array.push_back(va);
		index_array.push_back(ia);

		// メッシュなら
		if (nodeInfo->kind == 1)
		{
			// 頂点データ数を読み取る
			const UINT* p_nVertex = static_cast<const UINT*>(head);
			head = p_nVertex + 1;
			// 頂点データ数
			UINT nVertex = *p_nVertex;
			vertex_array[i].reserve(nVertex);
			size_t vbBytes = sizeof(VERTEX_FILE)* nVertex;
			// 頂点データを読み取る
			const VERTEX_FILE* node_vertex_array = static_cast<const VERTEX_FILE*>(head);
			head = node_vertex_array + nVertex;

			for (UINT j = 0; j < nVertex; j++)
			{
				// 頂点データ１個分のアドレスを計算
				const VERTEX_FILE* vertex = &node_vertex_array[j];
				// 使うデータだけ取り出す（Normal,UVは不要）
				VERTEX_LANDSHAPE vertex_landshape;
				// コピー
				vertex_landshape.Pos = vertex->Pos;
				vertex_landshape.Normal = vertex->Normal;
				// 頂点データを配列に格納
				vertex_array[i].push_back(vertex_landshape);
			}

			//インデックスデータ数を読み取る
			const UINT* p_nIndices = static_cast<const UINT*>(head);
			head = p_nIndices + 1;
			//インデックスデータ数
			UINT nIndices = *p_nIndices;
			index_array[i].reserve(nIndices);

			size_t ibBytes = sizeof(USHORT)* nIndices;
			// インデックスデータを読み取る
			const USHORT* node_index_array = static_cast<const USHORT*>(head);
			head = node_index_array + nIndices;

			for (UINT j = 0; j < nIndices; j++)
			{
				// インデックス１個分のアドレスを取得
				const USHORT* index = &node_index_array[j];
				// インデックスを配列に格納
				index_array[i].push_back(*index);
			}
		}
	}

	// 全ての頂点データをモデル座標系に変換
	UpdateNodeMatrices(0, nodeInfo_array, nullptr, vertex_array);

	// 全ノードの頂点データを配列に格納
	landshape->m_Vertices.clear();
	landshape->m_Indices.clear();
	int vertex_count = 0;
	for (UINT i = 0; i < nNode; i++)
	{
		vector<USHORT>::iterator it;

		// インデックスを全てのノードの通し番号に変換する
		for (it = index_array[i].begin(); it != index_array[i].end(); it++)
		{
			*it += vertex_count;
		}

		vertex_count += vertex_array[i].size();

		// 最後尾に連結する
		landshape->m_Vertices.insert(landshape->m_Vertices.end(), vertex_array[i].begin(), vertex_array[i].end());
		landshape->m_Indices.insert(landshape->m_Indices.end(), index_array[i].begin(), index_array[i].end());
	}

	return landshape;
}

// モデルデータ読み込み
std::unique_ptr<LandShapeData> LandShapeData::CreateFromMDL(const wchar_t* szFileName)
{
	ifstream ifs;
	std::unique_ptr<char[]> data;

	// ファイルオープン
	ifs.open(szFileName, ios::in | ios::binary);

	// 読み込み失敗
	assert(ifs && "CreateFromMDL Error: Failed to open MDL.");

	// ファイルサイズを取得
	ifs.seekg(0, fstream::end);
	streamoff eofPos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, fstream::beg);
	streamoff begPos = ifs.tellg();
	streamoff size = eofPos - begPos;

	// 読み込むためのメモリを確保（エラーチェックはしていない）
	data.reset(new char[(UINT)size]);

	// ファイル先頭からバッファへコピー 
	ifs.read(data.get(), size);

	// ファイルクローズ
	ifs.close();

	OutputDebugStringW(szFileName);
	OutputDebugStringW(L"\n");

	// 読み込んだデータからモデルデータを生成
	auto model = CreateFromMDL(data.get());

	// 名前取得
	model->name = szFileName;

	return model;
}

//--------------------------------------------------------------------------------------
// 各ノードの頂点を親の行列で変換する
//--------------------------------------------------------------------------------------
void LandShapeData::UpdateNodeMatrices(int index, const NodeInfo* nodeInfo_array, Matrix* pParentMatrix, vector<vector<VERTEX_LANDSHAPE>>& vertexArray)
{
	// ノード情報１個分を取得
	const NodeInfo* nodeInfo = &nodeInfo_array[index];

	// モデル行列を作成
	Matrix modelm;
	// 回転行列
	Matrix rotm = Matrix::CreateFromQuaternion(nodeInfo->rotation);
	// 平行移動行列
	Matrix transm = Matrix::CreateTranslation(nodeInfo->translation);

	// 行列を合成
	modelm *= rotm;
	modelm *= transm;

	// 親行列があれば、かける
	if (pParentMatrix != nullptr)
	{
		modelm = modelm * (*pParentMatrix);
	}

	// メッシュなら
	if (nodeInfo->kind == 1)
	{
		vector<VERTEX_LANDSHAPE>::iterator it;

		for (it = vertexArray[index].begin(); it != vertexArray[index].end(); it++)
		{
			// 座標を変換（ノード座標系→モデル座標系）
			it->Pos = Vector3::Transform(it->Pos, modelm);
			it->Normal = Vector3::TransformNormal(it->Normal, modelm);

			char str[256];

			// 変換後の頂点座標をログ出力
			sprintf_s(str, "(%.3f,%.3f,%.3f)\n", it->Pos.x, it->Pos.y, it->Pos.z);
			// 変換後の法線をログ出力
			//sprintf_s(str, "(%.3f,%.3f,%.3f)\n", it->Normal.x, it->Normal.y, it->Normal.z);

			OutputDebugStringA(str);
		}
	}

	// 兄弟ノードは、自分と共通の親行列で変換
	if (nodeInfo->sibling > 0)
	{
		// 再帰呼び出し
		UpdateNodeMatrices(nodeInfo->sibling, nodeInfo_array, pParentMatrix, vertexArray);
	}

	// 子ノードは自分を親行列として変換
	if (nodeInfo->child > 0)
	{
		// 再帰呼び出し
		UpdateNodeMatrices(nodeInfo->child, nodeInfo_array, &modelm, vertexArray);
	}
}