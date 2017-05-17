#include "Obj3D.h"
#include "VertexTypes.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

// �ÓI�����o�ϐ��̎���
ID3D11Device* Obj3D::s_pDevice;
ID3D11DeviceContext* Obj3D::s_pDeviceContext;
DirectX::CommonStates*	Obj3D::s_pStates;
DirectX::EffectFactory* Obj3D::s_pEffectFactory;
Camera* Obj3D::s_pCamera;
std::map<std::wstring, std::unique_ptr<DirectX::Model>> Obj3D::s_modelarray;

void Obj3D::StaticInitialize(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, DirectX::CommonStates * pStates, DirectX::EffectFactory * pfx, Camera * pCamera)
{
	SetDevice(pDevice);
	SetDeviceContext(pDeviceContext);
	SetStates(pStates);
	SetEffectFactory(pfx);
	SetCamera(pCamera);
}

/**
*	@brief �R���X�g���N�^
*/
Obj3D::Obj3D()
: m_pParentMatrix(nullptr)
, m_pModel(nullptr)
, m_UseQuternion(false)
{
	// �X�P�[����1�{���f�t�H���g
	m_Scale = Vector3(1, 1, 1);
}

/**
*	@brief �f�X�g���N�^
*/
Obj3D::~Obj3D()
{
}

/**
*	@brief �t�@�C�����烂�f����ǂݍ���
*/
void Obj3D::LoadModelFile(const wchar_t*filename)
{
	assert(s_pEffectFactory);

	// �������O�̃��f����ǂݍ��ݍς݂łȂ���΂Ђ�
	if (s_modelarray.count(filename) == 0 )
	{
		// ���f����ǂݍ��݁A�R���e�i�ɓo�^�i�L�[�̓t�@�C�����j
		s_modelarray[filename] = Model::CreateFromCMO(s_pDevice, filename, *s_pEffectFactory);
	}
	m_pModel = s_modelarray[filename].get();
}

/**
*	@brief �I�u�W�F�N�g�̃��C�e�B���O�𖳌��ɂ���
*/
void Obj3D::DisableLighting()
{
	if (m_pModel)
	{
		// ���f�����̑S���b�V������
		ModelMesh::Collection::const_iterator it_mesh = m_pModel->meshes.begin();
		for (; it_mesh != m_pModel->meshes.end(); it_mesh++)
		{
			ModelMesh* modelmesh = it_mesh->get();
			assert(modelmesh);

			// ���b�V�����̑S���b�V���p�[�c����
			std::vector<std::unique_ptr<ModelMeshPart>>::iterator it_meshpart = modelmesh->meshParts.begin();
			for (; it_meshpart != modelmesh->meshParts.end(); it_meshpart++)
			{
				ModelMeshPart* meshpart = it_meshpart->get();
				assert(meshpart);

				// ���b�V���p�[�c�ɃZ�b�g���ꂽ�G�t�F�N�g��BasicEffect�Ƃ��Ď擾
				BasicEffect* eff = static_cast<BasicEffect*>(meshpart->effect.get());
				if (eff != nullptr)
				{
					eff->SetLightingEnabled(false);
					XMVECTOR emissive;
					emissive.m128_f32[0] = 1.0f;
					emissive.m128_f32[1] = 1.0f;
					emissive.m128_f32[2] = 1.0f;
					emissive.m128_f32[3] = 1.0f;
					//eff->SetEmissiveColor(emissive);
					//eff->SetEmissiveColor(Vector3(0.5f, 0.5f, 0.5f));
					//eff->EnableDefaultLighting();
					eff->SetAmbientLightColor(Vector3(1,1,1));
					eff->SetDiffuseColor(Vector3(1, 1, 1));
					// �G�t�F�N�g�̊܂ޑS�Ă̕��s��������
					//for (int i = 0; i < BasicEffect::MaxDirectionalLights; i++)
					//{
					//	// ���C�g�𖳌��ɂ���
					//	eff->SetLightEnabled(i, false);
					//}
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

	// ���[���h�s���SRT�̏��ɍ���
	m_LocalWorld = Matrix::Identity;
	m_LocalWorld *= scalem;
	m_LocalWorld *= rotm;
	m_LocalWorld *= transm;

	// �e�s�񂪂����
	if ( m_pParentMatrix )
	{
		// �e�s����|����
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