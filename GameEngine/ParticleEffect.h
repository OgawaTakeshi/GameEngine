/// <summary>
/// パーティクルエフェクト
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

	// シングルトンインスタンスの取得
	static ParticleEffectManager* getInstance();
	// 破棄
	void Dispose();
	// 発生させる
	void Entry(const wchar_t*filename, int frame, Vector3 position, Vector3 velocity, Vector3 accel, Vector3 start_rotation, Vector3 end_rotation, Vector3 start_scale, Vector3 end_scale, Color start_color, Color end_color);
	void Update();
	void Draw();
private:
	ParticleEffectManager();	

	// エフェクト1個分
	class ParticleEffect
	{
	public:
		ParticleEffect()
		{
			frame = 0;
			num_frame = 0;
		}

		bool Update(const Vector3& gravity);

		//// オブジェクト
		//Obj3D obj;
		// 座標
		Vector3 position;
		// 速度
		Vector3 velocity;
		// 加速度
		Vector3 accel;
		// 初期値
		Vector3 s_rotation;
		Vector3 s_scale;
		Color s_color;
		// 最終値
		Vector3 e_rotation;
		Vector3 e_scale;
		Color e_color;
		// 現在フレーム
		int   frame;
		// 終了フレーム
		int   num_frame;
	};

	// モデルエフェクトの配列
	std::list<ParticleEffect> ParticleEffects;
	// 重力加速度
	Vector3 gravity;
};