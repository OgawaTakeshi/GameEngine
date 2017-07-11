#pragma once

#include "Camera.h"
#include "LockOn.h"
#include "FollowCamera.h"

// ロックオンカメラ
class LockOnCamera : public Camera
{
	// 状態
	enum State
	{
		LOCKON,
		FOLLOW,

		Num
	};
public:
	// 距離
	const float CAMERA_DISTANCE = 5.0f;
	// 切り替え時間
	const int SHIFT_FRAME = 120;
	// コンストラクタ
	LockOnCamera();
	// 更新
	void Update() override;
	//setter
	void SetLockOn(LockOn* lockOn) { m_LockOn = lockOn; }
protected:
	State m_State;
	// ロックオン
	LockOn* m_LockOn;
	// 追従カメラ
	std::unique_ptr<FollowCamera> m_FollowCamera;

	int ShiftCount;
};
