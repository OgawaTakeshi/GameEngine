#pragma once

#include <Model.h>
#include <Effects.h>

#include "Camera.h"
#include <map>

// 3D�I�u�W�F�N�g
class Obj3D
{
public:
	// �ÓI�����o�֐�
	// �ÓI������
	static void StaticInitialize(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, DirectX::CommonStates* pStates, DirectX::EffectFactory* pfx, Camera* pCamera);
	// �f�o�C�X��setter
	static void SetDevice(ID3D11Device* pDevice) { s_pDevice = pDevice; }
	// �f�o�C�X�R���e�L�X�g��setter
	static void SetDeviceContext(ID3D11DeviceContext* pDeviceContext) { s_pDeviceContext = pDeviceContext; }
	// �`��X�e�[�g��setter
	static void SetStates(DirectX::CommonStates* pStates) { s_pStates = pStates; }
	// �G�t�F�N�g�t�@�N�g����setter
	static void SetEffectFactory(DirectX::EffectFactory* pfx) { s_pEffectFactory = pfx; }
	// �J������setter
	static void SetCamera(Camera* pCamera) { s_pCamera = pCamera; }

	static ID3D11DeviceContext* GetDeviceContext() { return s_pDeviceContext; }
	// ���Z�`��ݒ���Z�b�g
	static void SetSubtractive();

private:
	// �ÓI�����o�ϐ��i�S�I�u�W�F�N�g�ŋ��L�j
	// �f�o�C�X�ւ̃|�C���^
	static ID3D11Device*	s_pDevice;
	// �f�o�C�X�R���e�L�X�g�ւ̃|�C���^
	static ID3D11DeviceContext*	s_pDeviceContext;
	// �`��X�e�[�g�ւ̃|�C���^
	static DirectX::CommonStates*	s_pStates;
	// ���p�̃G�t�F�N�g�t�@�N�g��
	static DirectX::EffectFactory*	s_pEffectFactory;
	// ���p�̃J�����i�`�掞�Ɏg�p�j
	static Camera* s_pCamera;
	// �ǂݍ��ݍς݃��f���R���e�i
	static std::map<std::wstring, std::unique_ptr<DirectX::Model>> s_modelarray;

	static ID3D11BlendState* s_pBlendStateSubtract;
public:
	Obj3D();
	virtual ~Obj3D();
	// �t�@�C�����烂�f����ǂݍ���
	void LoadModelFile(const wchar_t*filename=nullptr);
	// �s��̌v�Z
	void Calc();
	// �`��
	void Draw();
	// ���Z�`��ł̕`��i�e�p�j
	void DrawSubtractive();
	// �I�u�W�F�N�g�̃��C�e�B���O�𖳌��ɂ���
	void DisableLighting();
	
	// �A�N�Z�b�T
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
	// ���f���f�[�^�ւ̃|�C���^
	const DirectX::Model* m_pModel;
	// ���s�ړ�
	DirectX::SimpleMath::Vector3 m_Trans;
	// ��]
	bool m_UseQuternion;
	DirectX::SimpleMath::Vector3 m_Rot;
	DirectX::SimpleMath::Quaternion m_RotQ;
	// �X�P�[�����O
	DirectX::SimpleMath::Vector3 m_Scale;
	// ���[���h�s��
	DirectX::SimpleMath::Matrix m_LocalWorld;
	// �e�̃��[���h�s��ւ̃|�C���^
	Obj3D* m_pParent;
};
