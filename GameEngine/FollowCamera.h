/// <summary>
/// 追従カメラ
/// </summary>
#pragma once

#include "Camera.h"
#include "Player.h"

// カメラ
class FollowCamera : public Camera
{
public:
	// コンストラクタ
	FollowCamera(int width, int height);
	// デストラクタ
	virtual ~FollowCamera();
	// 更新
	void Update() override;
	void Update(bool interpolate);

	void SetTarget(Player* target) { m_Target = target; }

	void SetTargetPos(DirectX::SimpleMath::Vector3 target);

	void SetTargetAngle(float target);
private:
	Player* m_Target;
	// ターゲット座標
	DirectX::SimpleMath::Vector3 m_TargetPos;
	// ターゲット角度
	float m_TargetAngle;
	// 距離
	float m_Distance;
	// 参照点オフセット
	DirectX::SimpleMath::Vector3 m_Offset;
};
