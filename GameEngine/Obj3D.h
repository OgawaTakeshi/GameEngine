#pragma once

#include <Model.h>
#include <Effects.h>

#include "Camera.h"
#include <map>

// 3Dオブジェクト
class Obj3D
{
public:
	// 静的メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, DirectX::CommonStates* pStates, DirectX::EffectFactory* pfx, Camera* pCamera);
	// デバイスのsetter
	static void SetDevice(ID3D11Device* pDevice) { s_pDevice = pDevice; }
	// デバイスコンテキストのsetter
	static void SetDeviceContext(ID3D11DeviceContext* pDeviceContext) { s_pDeviceContext = pDeviceContext; }
	// 描画ステートのsetter
	static void SetStates(DirectX::CommonStates* pStates) { s_pStates = pStates; }
	// エフェクトファクトリのsetter
	static void SetEffectFactory(DirectX::EffectFactory* pfx) { s_pEffectFactory = pfx; }
	// カメラのsetter
	static void SetCamera(Camera* pCamera) { s_pCamera = pCamera; }

	static ID3D11DeviceContext* GetDeviceContext() { return s_pDeviceContext; }
	// 減算描画設定をセット
	static void SetSubtractive();

private:
	// 静的メンバ変数（全オブジェクトで共有）
	// デバイスへのポインタ
	static ID3D11Device*	s_pDevice;
	// デバイスコンテキストへのポインタ
	static ID3D11DeviceContext*	s_pDeviceContext;
	// 描画ステートへのポインタ
	static DirectX::CommonStates*	s_pStates;
	// 共用のエフェクトファクトリ
	static DirectX::EffectFactory*	s_pEffectFactory;
	// 共用のカメラ（描画時に使用）
	static Camera* s_pCamera;
	// 読み込み済みモデルコンテナ
	static std::map<std::wstring, std::unique_ptr<DirectX::Model>> s_modelarray;

	static ID3D11BlendState* s_pBlendStateSubtract;
public:
	Obj3D();
	virtual ~Obj3D();
	// ファイルからモデルを読み込む
	void LoadModelFile(const wchar_t*filename=nullptr);
	// 行列の計算
	void Calc();
	// 描画
	void Draw();
	// 減算描画での描画（影用）
	void DrawSubtractive();
	// オブジェクトのライティングを無効にする
	void DisableLighting();
	
	// アクセッサ
	void SetTrans(const DirectX::SimpleMath::Vector3& trans) { m_Trans = trans; }
	void SetRot(const DirectX::SimpleMath::Vector3& rot)	 { m_Rot = rot; m_UseQuternion = false; }
	void SetRotQ(const DirectX::SimpleMath::Quaternion& rotq)	 { m_RotQ = rotq; m_UseQuternion = true; }
	void SetScale(const DirectX::SimpleMath::Vector3& scale) { m_Scale = scale; }
	void SetLocalWorld(const DirectX::SimpleMath::Matrix& mat) { m_LocalWorld = mat; }
	void SetParent(Obj3D* pParent)	{ m_pParent = pParent; }

	const DirectX::SimpleMath::Vector3& GetTrans()	{ return m_Trans; }
	const DirectX::SimpleMath::Vector3& GetRot()	{ return m_Rot; }
	const DirectX::SimpleMath::Vector3& GetScale()	{ return m_Scale; }
	const DirectX::SimpleMath::Matrix& GetLocalWorld() { return m_LocalWorld; }

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
	DirectX::SimpleMath::Matrix m_LocalWorld;
	// 親のワールド行列へのポインタ
	Obj3D* m_pParent;
};
