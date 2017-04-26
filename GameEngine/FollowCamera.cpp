/**
*	@file	Camera.cpp
*
*	@brief	カメラ
*
*	@date	2012/04/23
*
*	@author	藤澤　伸治
*/
#include "FollowCamera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

/**
*	@brief コンストラクタ
*/
FollowCamera::FollowCamera()
: m_FovY(XMConvertToRadians(60.0f))
, m_Aspect(640.0f/480.0f)
, m_NearClip(0.1f)
, m_FarClip(1000.0f)
{
	m_Viewmat= Matrix::Identity;
	m_Eyepos = Vector3(0.0f,6.0f, 10.0f);
	m_Refpos = Vector3(0.0f, 2.0f, 0.0f);
	m_Upvec = Vector3(0.0f, 1.0f, 0.0f);
	m_Projmat = Matrix::Identity;
}

/**
*	@brief デストラクタ
*/
FollowCamera::~FollowCamera()
{
}

/**
*	@brief 更新
*/
void FollowCamera::Update()
{
	// ビュー行列を計算
	m_Viewmat = Matrix::CreateLookAt(m_Eyepos, m_Refpos, m_Upvec);
	// プロジェクション行列を計算
	m_Projmat = Matrix::CreatePerspectiveFieldOfView(m_FovY, m_Aspect, m_NearClip, m_FarClip);
}