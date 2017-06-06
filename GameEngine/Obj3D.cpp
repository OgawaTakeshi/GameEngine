#include "Obj3D.h"
#include "VertexTypes.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// 静的メンバ変数の実体
ID3D11Device* Obj3D::s_pDevice;
ID3D11DeviceContext* Obj3D::s_pDeviceContext;
DirectX::CommonStates*	Obj3D::s_pStates;
DirectX::EffectFactory* Obj3D::s_pEffectFactory;
Camera* Obj3D::s_pCamera;
std::map<std::wstring, std::unique_ptr<DirectX::Model>> Obj3D::s_modelarray;

static int mode = 0;

void Obj3D::StaticInitialize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, DirectX::CommonStates * pStates, DirectX::EffectFactory * pfx, Camera * pCamera)
{
	SetDevice(pDevice);
	SetDeviceContext(pDeviceContext);
	SetStates(pStates);
	SetEffectFactory(pfx);
	SetCamera(pCamera);
}

/**
*	@brief コンストラクタ
*/
Obj3D::Obj3D()
: m_pParentMatrix(nullptr)
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

void Obj3D::UpdateOnce()
{
	if (Game::m_keyboardTracker.IsKeyPressed(Keyboard::Keys::Enter))
	{
		mode++;
		if (mode >= 6) mode = 0;
	}

	Game::m_debugText->AddText(Vector2(20, 120), L"Enter: Change Order");

	static int y_order = 140;

	switch (mode)
	{

	case 0:
		Game::m_debugText->AddText(Vector2(20, y_order), L"Z > X > Y");
		break;
	case 1:
		Game::m_debugText->AddText(Vector2(20, y_order), L"Z > Y > X");
		break;
	case 2:
		Game::m_debugText->AddText(Vector2(20, y_order), L"X > Y > Z");
		break;
	case 3:
		Game::m_debugText->AddText(Vector2(20, y_order), L"X > Z > Y");
		break;
	case 4:
		Game::m_debugText->AddText(Vector2(20, y_order), L"Y > X > Z");
		break;
	case 5:
		Game::m_debugText->AddText(Vector2(20, y_order), L"Y > Z > X");
		break;

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
		Matrix rotx = Matrix::CreateRotationX(m_Rot.x);
		Matrix roty = Matrix::CreateRotationY(m_Rot.y);
		Matrix rotz = Matrix::CreateRotationZ(m_Rot.z);
		switch (mode)
		{

		case 0:
			rotm = rotz * rotx * roty;
			break;
		case 1:
			rotm = rotz * roty * rotx;
			break;
		case 2:
			rotm = rotx * roty * rotz;
			break;
		case 3:
			rotm = rotx * rotz * roty;
			break;
		case 4:
			rotm = roty * rotx * rotz;
			break;
		case 5:
			rotm = roty * rotz * rotx;
			break;

		}
		
	}
	
	transm = Matrix::CreateTranslation(m_Trans);

	// ワールド行列をSRTの順に合成
	m_LocalWorld = Matrix::Identity;
	m_LocalWorld *= scalem;
	m_LocalWorld *= rotm;
	m_LocalWorld *= transm;

	// 親行列があれば
	if ( m_pParentMatrix )
	{
		// 親行列を掛ける
		m_LocalWorld = m_LocalWorld * (*m_pParentMatrix);
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