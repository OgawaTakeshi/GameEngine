#include "LockOnCamera.h"

LockOnCamera::LockOnCamera(int width, int height)
	: Camera(width, height)
{
	m_FollowCamera = std::make_unique<FollowCamera>(width, height);
	ShiftCount = 0;

	m_State = State::FOLLOW;
}

void LockOnCamera::Update()
{
	// 追従カメラのターゲットを自機に設定
	m_FollowCamera->SetTarget(m_LockOn->GetPlayer());
	// 追従カメラの更新
	m_FollowCamera->Update(false);

	bool isLockOn = m_LockOn->IsLockOn();

	Enemy* enemy = nullptr;

	// ロックオン中？
	if (isLockOn)
	{
		enemy = m_LockOn->SearchLockingEnemy();
		if (!enemy)
		{
			isLockOn = false;
		}
	}

	// ステートの切り替え
	switch (m_State)
	{
	case LOCKON:
		if (!isLockOn)
		{
			m_State = FOLLOW;
		}
		break;
	case FOLLOW:
	default:
		if (isLockOn)
		{
			m_State = LOCKON;
			ShiftCount = SHIFT_FRAME;
		}
		break;
	}

	// ロックオン中？
	if (isLockOn)
	{
		Vector3 eyeposPre = GetEyepos();
		Vector3 refposPre = GetRefpos();

		Player* player = m_LockOn->GetPlayer();
		Enemy* enemy = m_LockOn->SearchLockingEnemy();
		if (enemy)
		{
			const Vector3& playerTrans = player->GetTrans();
			const Vector3& enemyTrans = enemy->GetTrans();
			Vector3 eyepos;
			Vector3 refpos;

			const float CAMERA_UPOFFSET = 2.0f;

			// 敵から自機の頭上への方向ベクトル
			// 自機と敵が近づくほど、上から見下ろす状態になる
			Vector3 cameraV = playerTrans + Vector3(0, CAMERA_UPOFFSET, 0) - enemyTrans;
			cameraV.Normalize();
			cameraV *= CAMERA_DISTANCE;

			// 視点、参照点
			eyepos = playerTrans + cameraV;
			// 一定以上の高さの差を保証
			if (eyepos.y - playerTrans.y < CAMERA_UPOFFSET)
			{
				eyepos.y = playerTrans.y + CAMERA_UPOFFSET;
			}
			refpos = enemyTrans;

			// 移行期間
			if (ShiftCount > 0)
			{
				// 移行期間の終わりが近づくにつれ、補間なし状態に近づける
				float eyepos_ratio = 0.95f * ShiftCount / SHIFT_FRAME;
				float rafpos_ratio = 0.80f * ShiftCount / SHIFT_FRAME;
				eyepos = eyepos + (eyeposPre - eyepos) * eyepos_ratio;
				refpos = refpos + (refposPre - refpos) * rafpos_ratio;
				ShiftCount--;
			}

			this->SetEyepos(eyepos);
			this->SetRefpos(refpos);
			this->SetUpvec(Vector3::UnitY);
		}
		else
		{
			// ロックオン中の敵がもういない
			isLockOn = false;
		}
	}
	
	if (!isLockOn)
	{
		Vector3 eyeposPre = GetEyepos();
		Vector3 refposPre = GetRefpos();

		Vector3 eyepos = eyeposPre + (m_FollowCamera->GetEyepos() - eyeposPre) * 0.05f;
		Vector3 refpos = refposPre + (m_FollowCamera->GetRefpos() - refposPre) * 0.20f;

		this->SetEyepos(eyepos);
		this->SetRefpos(refpos);
		this->SetUpvec(m_FollowCamera->GetUpvec());
	}

	Camera::Update();
}
