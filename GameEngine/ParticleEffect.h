/// <summary>
/// �p�[�e�B�N���G�t�F�N�g
/// </summary>
#pragma once

#include <d3d11_1.h>
#include <SimpleMath.h>
#include <list>
#include <map>
#include "Obj3D.h"

class ParticleEffectManager
{
public:
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Color = DirectX::SimpleMath::Color;

	// �V���O���g���C���X�^���X�̎擾
	static ParticleEffectManager* getInstance();
	// �j��
	void Dispose();
	// ����������
	void Entry(const wchar_t*filename, int frame, Vector3 position, Vector3 velocity, Vector3 accel, Vector3 start_rotation, Vector3 end_rotation, Vector3 start_scale, Vector3 end_scale, Color start_color, Color end_color);
	void Update();
	void Draw();
private:
	ParticleEffectManager();	

	// �G�t�F�N�g1��
	class ParticleEffect
	{
	public:
		ParticleEffect()
		{
			frame = 0;
			num_frame = 0;
		}

		bool Update(const Vector3& gravity);

		//// �I�u�W�F�N�g
		//Obj3D obj;
		// ���W
		Vector3 position;
		// ���x
		Vector3 velocity;
		// �����x
		Vector3 accel;
		// �����l
		Vector3 s_rotation;
		Vector3 s_scale;
		Color s_color;
		// �ŏI�l
		Vector3 e_rotation;
		Vector3 e_scale;
		Color e_color;
		// ���݃t���[��
		int   frame;
		// �I���t���[��
		int   num_frame;
	};

	// ���f���G�t�F�N�g�̔z��
	std::list<ParticleEffect> ParticleEffects;
	// �d�͉����x
	Vector3 gravity;
};