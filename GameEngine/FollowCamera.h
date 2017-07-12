/// <summary>
/// �Ǐ]�J����
/// </summary>
#pragma once

#include "Camera.h"
#include "Player.h"

// �J����
class FollowCamera : public Camera
{
public:
	// �R���X�g���N�^
	FollowCamera(int width, int height);
	// �f�X�g���N�^
	virtual ~FollowCamera();
	// �X�V
	void Update() override;
	void Update(bool interpolate);

	void SetTarget(Player* target) { m_Target = target; }

	void SetTargetPos(DirectX::SimpleMath::Vector3 target);

	void SetTargetAngle(float target);
private:
	Player* m_Target;
	// �^�[�Q�b�g���W
	DirectX::SimpleMath::Vector3 m_TargetPos;
	// �^�[�Q�b�g�p�x
	float m_TargetAngle;
	// ����
	float m_Distance;
	// �Q�Ɠ_�I�t�Z�b�g
	DirectX::SimpleMath::Vector3 m_Offset;
};
