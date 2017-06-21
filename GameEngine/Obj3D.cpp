#include "Obj3D.h"
#include "VertexTypes.h"
#include <CommonStates.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// 静的メンバ変数の実体
ID3D11Device* Obj3D::s_pDevice;
ID3D11DeviceContext* Obj3D::s_pDeviceContext;
std::unique_ptr<DirectX::CommonStates>	Obj3D::s_pStates;
std::unique_ptr<DirectX::EffectFactory>	Obj3D::s_pEffectFactory;
Camera* Obj3D::s_pCamera;
std::map<std::wstring, std::unique_ptr<DirectX::Model>> Obj3D::s_modelarray;
ID3D11BlendState* Obj3D::s_pBlendStateSubtract;

void Obj3D::StaticInitialize(const Defs& def)
{
	SetDevice(def.pDevice);
	SetDeviceContext(def.pDeviceContext);
	SetCamera(def.pCamera);

	// エフェクトファクトリ生成
	s_pEffectFactory = std::make_unique<EffectFactory>(def.pDevice);
	s_pEffectFactory->SetDirectory(L"Resources");

	// 汎用ステート生成
	s_pStates = std::make_unique<CommonStates>(def.pDevice);

	// 減算描画用のブレンドステートを作成
	D3D11_BLEND_DESC desc;
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_REV_SUBTRACT;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT ret = s_pDevice->CreateBlendState(&desc, &s_pBlendStateSubtract);
}

void Obj3D::SetSubtractive()
{
	// 減算描画を設定
	s_pDeviceContext->OMSetBlendState(s_pBlendStateSubtract, nullptr, 0xffffff);
}

/**
*	@brief コンストラクタ
*/
Obj3D::Obj3D()
: m_pParent(nullptr)
, m_pModel(nullptr)
, m_UseQuternion(false)
{
	// スケールは1倍がデフォルト
	m_Scale = Vector3(1, 1, 1);
}

/**
*	@brief デストラクタ
*/
Obj3D::~Obj3D()
{
}

/**
*	@brief ファイルからモデルを読み込む
*/
void Obj3D::LoadModelFile(const wchar_t*filename)
{
	assert(s_pEffectFactory);

	// 同じ名前のモデルを読み込み済みでなければひｔ
	if (s_modelarray.count(filename) == 0 )
	{
		// モデルを読み込み、コンテナに登録（キーはファイル名）
		s_modelarray[filename] = Model::CreateFromCMO(s_pDevice, filename, *s_pEffectFactory);
	}
	m_pModel = s_modelarray[filename].get();
}

/**
*	@brief オブジェクトのライティングを無効にする
*/
void Obj3D::DisableLighting()
{
	if (m_pModel)
	{
		// モデル内の全メッシュ分回す
		ModelMesh::Collection::const_iterator it_mesh = m_pModel->meshes.begin();
		for (; it_mesh != m_pModel->meshes.end(); it_mesh++)
		{
			ModelMesh* modelmesh = it_mesh->get();
			assert(modelmesh);

			// メッシュ内の全メッシュパーツ分回す
			std::vector<std::unique_ptr<ModelMeshPart>>::iterator it_meshpart = modelmesh->meshParts.begin();
			for (; it_meshpart != modelmesh->meshParts.end(); it_meshpart++)
			{
				ModelMeshPart* meshpart = it_meshpart->get();
				assert(meshpart);

				// メッシュパーツにセットされたエフェクトをBasicEffectとして取得
				std::shared_ptr<IEffect> ieff = meshpart->effect;
				BasicEffect* eff = dynamic_cast<BasicEffect*>(ieff.get());
				if (eff != nullptr)
				{
					// 自己発光を最大値に
					eff->SetEmissiveColor(Vector3(1,1,1));

					// エフェクトに含む全ての平行光源分について処理する
					for (int i = 0; i < BasicEffect::MaxDirectionalLights; i++)
					{
						// ライトを無効にする
						eff->SetLightEnabled(i, false);
					}
				}
			}
		}
	}
}

void Obj3D::Calc()
{
	Matrix scalem;
	Matrix rotm;
	Matrix transm;

	scalem = Matrix::CreateScale(m_Scale);

	if (m_UseQuternion)
	{
		rotm = Matrix::CreateFromQuaternion(m_RotQ);
	}
	else
	{
		rotm = Matrix::CreateFromYawPitchRoll(m_Rot.y, m_Rot.x, m_Rot.z);
	}
	
	transm = Matrix::CreateTranslation(m_Trans);

	// ワールド行列をSRTの順に合成
	m_LocalWorld = Matrix::Identity;
	m_LocalWorld *= scalem;
	m_LocalWorld *= rotm;
	m_LocalWorld *= transm;

	// 親行列があれば
	if ( m_pParent )
	{
		// 親行列を掛ける
		m_LocalWorld = m_LocalWorld * m_pParent->GetLocalWorld();
	}
}

void Obj3D::Draw()
{
	if ( m_pModel )
	{
		assert(s_pCamera);
		const Matrix& view = s_pCamera->GetViewmat();
		const Matrix& projection = s_pCamera->GetProjmat();

		assert(s_pDeviceContext);
		assert(s_pStates);

		m_pModel->Draw(s_pDeviceContext, *s_pStates, m_LocalWorld, view, projection);
	}
}

void Obj3D::DrawSubtractive()
{
	if (m_pModel)
	{
		assert(s_pCamera);
		const Matrix& view = s_pCamera->GetViewmat();
		const Matrix& projection = s_pCamera->GetProjmat();

		assert(s_pDeviceContext);
		assert(s_pStates);

		// 減算描画用の設定関数を渡して描画
		m_pModel->Draw(s_pDeviceContext, *s_pStates, m_LocalWorld, view, projection, false, Obj3D::SetSubtractive);
	}
}