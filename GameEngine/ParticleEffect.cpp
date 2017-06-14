#include "ParticleEffect.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

ParticleEffectManager * ParticleEffectManager::getInstance()
{
	static ParticleEffectManager instance;

	return &instance;
}

void ParticleEffectManager::Dispose()
{
	ParticleEffects.clear();
}

void ParticleEffectManager::Entry(
	const wchar_t*filename,	// �t�@�C����
	int frame,	// �t���[����
	Vector3 position,	// ���W
	Vector3 velocity,	// ���x
	Vector3 accel,	// �����x
	Vector3 start_rotation, Vector3 end_rotation, // ��]�E�J�n�p�x�ƏI���p�x
	Vector3 start_scale, Vector3 end_scale, // �g�k�E�J�n�{���ƏI���{��
	Color start_color, Color end_color	// �J�n���ƏI�����̐F
)
{
	ParticleEffect effect;

	//effect.obj.LoadModelFile(filename);

	effect.position = position;
	effect.velocity = velocity;
	effect.accel = accel;
	effect.s_rotation = start_rotation;
	effect.e_rotation = end_rotation;
	effect.s_scale = start_scale;
	effect.e_scale = end_scale;
	effect.s_color = start_color;
	effect.e_color = end_color;
	effect.frame = 0;
	effect.num_frame = frame;

	ParticleEffects.push_back(effect);

}

void ParticleEffectManager::Update()
{
	// �p�[�e�B�N����S�čX�V
	std::list<ParticleEffect>::iterator it;
	for( it = ParticleEffects.begin(); it != ParticleEffects.end(); )
	{
		// �X�V
		if (it->Update(gravity))
		{
			it++;
		}
		else
		{
			// �����ɒB�����p�[�e�B�N�����������
			it = ParticleEffects.erase(it);
		}
	}
}

void ParticleEffectManager::Draw()
{
	// �p�[�e�B�N����S�ĕ`��
	std::list<ParticleEffect>::iterator it;
	for (it = ParticleEffects.begin(); it != ParticleEffects.end(); it++)
	{
		//it->obj.Draw();
	}
}

ParticleEffectManager::ParticleEffectManager()
{
}

bool ParticleEffectManager::ParticleEffect::Update(const Vector3& gravity)
{
	// �o�߃t���[�������J�E���g
	frame++;
	if (frame > num_frame) return false;
	// �i�s�x��0�`1�͈̔͂Ɋ��Z
	float f = (float)num_frame / frame;

	// ���x�ɉ����x�����Z
	velocity += accel;

	// �d�͉����x�ɂ�����
	velocity += gravity;

	// ���x�ɂ��ړ�
	position += velocity;

	// �p�x�̐��`�⊮
	Vector3 rotation = (e_rotation - s_rotation) / f;
	rotation += s_rotation;
	rotation.x = XMConvertToRadians(rotation.x);
	rotation.y = XMConvertToRadians(rotation.y);
	rotation.z = XMConvertToRadians(rotation.z);

	// �X�P�[���̐��`���
	Vector3 scale = (e_scale - s_scale) / f;
	scale += s_scale;

	// �F�̐��`���
	Color color = (e_color - s_color) / f;
	color += s_color;

	//obj.SetTrans(position);
	//obj.SetRot(rotation);
	//obj.SetScale(scale);
	//obj.Calc();

	return true;
}
