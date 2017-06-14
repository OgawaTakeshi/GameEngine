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
	const wchar_t*filename,	// ファイル名
	int frame,	// フレーム数
	Vector3 position,	// 座標
	Vector3 velocity,	// 速度
	Vector3 accel,	// 加速度
	Vector3 start_rotation, Vector3 end_rotation, // 回転・開始角度と終了角度
	Vector3 start_scale, Vector3 end_scale, // 拡縮・開始倍率と終了倍率
	Color start_color, Color end_color	// 開始時と終了時の色
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
	// パーティクルを全て更新
	std::list<ParticleEffect>::iterator it;
	for( it = ParticleEffects.begin(); it != ParticleEffects.end(); )
	{
		// 更新
		if (it->Update(gravity))
		{
			it++;
		}
		else
		{
			// 寿命に達したパーティクルを解放する
			it = ParticleEffects.erase(it);
		}
	}
}

void ParticleEffectManager::Draw()
{
	// パーティクルを全て描画
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
	// 経過フレーム数をカウント
	frame++;
	if (frame > num_frame) return false;
	// 進行度を0～1の範囲に換算
	float f = (float)num_frame / frame;

	// 速度に加速度を加算
	velocity += accel;

	// 重力加速度による加速
	velocity += gravity;

	// 速度による移動
	position += velocity;

	// 角度の線形補完
	Vector3 rotation = (e_rotation - s_rotation) / f;
	rotation += s_rotation;
	rotation.x = XMConvertToRadians(rotation.x);
	rotation.y = XMConvertToRadians(rotation.y);
	rotation.z = XMConvertToRadians(rotation.z);

	// スケールの線形補間
	Vector3 scale = (e_scale - s_scale) / f;
	scale += s_scale;

	// 色の線形補間
	Color color = (e_color - s_color) / f;
	color += s_color;

	//obj.SetTrans(position);
	//obj.SetRot(rotation);
	//obj.SetScale(scale);
	//obj.Calc();

	return true;
}
