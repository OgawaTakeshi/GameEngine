/**
*	@file	FollowCamera.h
*
*	@brief	カメラ
*
*	@date	2012/04/23
*
*	@author	藤澤　伸治
*/
#pragma once

#include "Camera.h"
#include "Player.h"

// カメラ
class FollowCamera : public Camera
{
public:
	// コンストラクタ
	FollowCamera();
	// デストラクタ
	virtual ~FollowCamera();
	// 更新
	void Update(bool interpolate = true);

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
