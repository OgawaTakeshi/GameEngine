/// <summary>
/// ３Ｄオブジェクトクラス
/// </summary>
#pragma once

#include <Model.h>
#include <Effects.h>

#include "Camera.h"
#include <map>

// 3Dオブジェクト
class Obj3D
{
	/// <summary>
	/// static member
	/// </summary>
public:
	static const wstring RESOURCE_DIRECTORY;
	static const wstring RESOURCE_EXT;

	// 設定
	struct CommonDef
	{
		ID3D11Device* pDevice;
		ID3D11DeviceContext* pDeviceContext;
		Camera* pCamera;

		CommonDef()
		{
			pDevice = nullptr;
			pDeviceContext = nullptr;
			pCamera = nullptr;
		}
	};
	// 共用データ
	struct Common
	{
		ID3D11Device*	device;
		// デバイスコンテキストへのポインタ
		ID3D11DeviceContext*	deviceContext;
		// 描画ステートへのポインタ
		std::unique_ptr<DirectX::CommonStates>	states;
		// 共用のエフェクトファクトリ
		std::unique_ptr<DirectX::EffectFactory>	effectFactory;
		// 共用のカメラ（描画時に使用）
		Camera* camera;
		// 読み込み済みモデルコンテナ
		std::map<std::wstring, std::unique_ptr<DirectX::Model>> modelarray;
		// 減算描画ステート
		ID3D11BlendState* blendStateSubtract;
	};
	// 静的メンバ関数
	// システム初期化
	static void InitializeCommon(CommonDef def);
	// 全モデル破棄
	static void UnloadAll();

	// 減算描画設定をセット
	static void SetSubtractive();

	//getter
	static Camera* GetCamera() { return s_Common.camera; }

private:
	// 静的メンバ変数（全オブジェクトで共有）
	static Common s_Common;

	/// <summary>
	/// non-static member
	/// </summary>
public:
	Obj3D();
	// ファイルからモデルを読み込む
	void LoadModel(const wchar_t*filename=nullptr);
	// 行列の更新
	void Update();
	// 描画
	void Draw();
	// 減算描画での描画（影用）
	void DrawSubtractive();
	void DrawBillboard();
	void DrawBillboardConstrainY();
	// オブジェクトのライティングを無効にする
	void DisableLighting();

	void EnableAlpha();
	
	//setter
	void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }
	void SetRot(const DirectX::SimpleMath::Vector3& rot)	 { m_Rot = rot; m_UseQuternion = false; }
	void SetRotQ(const DirectX::SimpleMath::Quaternion& rotq)	 { m_RotQ = rotq; m_UseQuternion = true; }
	void SetScale(const DirectX::SimpleMath::Vector3& scale) { m_Scale = scale; }
	void SetLocalWorld(const DirectX::SimpleMath::Matrix& mat) { m_World = mat; }
	void SetParent(Obj3D* pParent)	{ m_pParent = pParent; }
	//getter
	const DirectX::SimpleMath::Vector3& GetTrans()	{ return m_Trans; }
	const DirectX::SimpleMath::Vector3& GetRot()	{ return m_Rot; }
	const DirectX::SimpleMath::Quaternion& GetRotQ()	{ return m_RotQ; }
	const DirectX::SimpleMath::Vector3& GetScale()	{ return m_Scale; }
	const DirectX::SimpleMath::Matrix& GetLocalWorld() { return m_World; }

private:
	// モデルデータへのポインタ
	const DirectX::Model* m_pModel;
	// 平行移動
	DirectX::SimpleMath::Vector3 m_Trans;
	// 回転
	bool m_UseQuternion;
	DirectX::SimpleMath::Vector3 m_Rot;
	DirectX::SimpleMath::Quaternion m_RotQ;
	// スケーリング
	DirectX::SimpleMath::Vector3 m_Scale;
	// ワールド行列
	DirectX::SimpleMath::Matrix m_World;
	// 親のワールド行列へのポインタ
	Obj3D* m_pParent;
};
