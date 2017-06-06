#pragma once

#include <vector>
#include <memory>
#include <d3d11_1.h>
#include <SimpleMath.h>

// �n�`������f�[�^
class LandShapeData
{
public:
	friend class LandShape;
	// ���_���\����(�K�v�Ȃ��̂̂݁j
	struct VERTEX_LANDSHAPE
	{
		DirectX::SimpleMath::Vector3  Pos;		// ���W
		DirectX::SimpleMath::Vector3  Normal;	// �@���x�N�g��
	};

	// ���_���\���́i�ǂݍ��݃f�[�^�j
	struct VERTEX_FILE
	{
		DirectX::SimpleMath::Vector3  Pos;		// ���W
		DirectX::SimpleMath::Vector3  Normal;	// �@���x�N�g��
		DirectX::SimpleMath::Vector2  UV;		// �e�N�Z��
	};

	// �m�[�h���
	struct NodeInfo
	{
		// �}�e���A�����\����
		struct Material
		{
			char name[32];
			DirectX::SimpleMath::Vector3 Ambient;
			DirectX::SimpleMath::Vector3 Diffuse;
			DirectX::SimpleMath::Vector3 Specular;
			DirectX::SimpleMath::Vector3 Emissive;
			float Opacity;
			float Shininess;
			float Reflectivity;
		};

		// ��� (0:bone 1:mesh)
		int kind;

		// ���O
		char name[32];

		// �Z��̃m�[�h�ԍ�
		int sibling;

		// �q���̃m�[�h�ԍ�
		int child;

		// ��{�p��
		DirectX::SimpleMath::Vector3 translation;
		DirectX::SimpleMath::Quaternion rotation;

		// �e�N�X�`����(Diffuse)
		char textureNameA[32];

		// �e�N�X�`����(Specular)
		char textureNameB[32];

		// �e�N�X�`����(Bump)
		char textureNameC[32];

		// �}�e���A�����
		Material material;
	};

	// ���O
	std::wstring name;

	// ���_�f�[�^�z��
	std::vector<VERTEX_LANDSHAPE> m_Vertices;
	// �C���f�b�N�X�f�[�^�z��
	std::vector<USHORT> m_Indices;

	// ���f����ǂݍ���
	static std::unique_ptr<LandShapeData> CreateFromMDL(const char* meshData);
	static std::unique_ptr<LandShapeData> CreateFromMDL(const wchar_t* szFileName);
	// ���W�ϊ�
	static void UpdateNodeMatrices(int index, const NodeInfo* nodeInfo_array, DirectX::SimpleMath::Matrix* pParentMatrix, std::vector<std::vector<VERTEX_LANDSHAPE>>& vertexArray);
};