//
// Game.h
//

#pragma once

#include <VertexTypes.h>
#include <PrimitiveBatch.h>
#include <SimpleMath.h>
#include <Effects.h>
#include <CommonStates.h>
#include <Model.h>

#include "DeviceResources.h"
#include "StepTimer.h"
#include "DebugCamera.h"
#include "DebugText.h"
#include "FollowCamera.h"
#include "Obj3D.h"
#include "Player.h"
#include "Enemy.h"
#include "LandShape.h"
#include "KeyboardUtil.h"
#include "MouseUtil.h"
#include "LockOnCamera.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:
	static Game* GetInstance();
private:
	static Game* m_Instance;
public:
	static const int ENEMY_NUM = 5;

    Game();

	~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

	LockOn* GetLockOn() { return m_LockOn.get(); }

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
	DX::DeviceResources*    m_deviceResources;

	// Rendering loop timer.
	DX::StepTimer                           m_timer;

	// デバッグテキスト
	std::unique_ptr<DebugText> m_debugText;

	// 天球
	std::unique_ptr<Obj3D> m_ObjSkydome;
	//// 地面
	//std::unique_ptr<LandShape> m_LandShapeGround;
	// 地形
	std::vector<std::unique_ptr<LandShape>>	m_pLandShapeArray;

	// ロックオン
	std::unique_ptr<LockOn> m_LockOn;

	// デバッグ用カメラ
	std::unique_ptr<DebugCamera> m_DebugCamera;
	// 追従カメラ
	std::unique_ptr<FollowCamera> m_FollowCamera;
	// ロックオンカメラ
	std::unique_ptr<LockOnCamera> m_LockOnCamera;

	// 現在のカメラ
	Camera* m_CurrentCamera;

	// 自機
	std::unique_ptr<Player> m_Player;

	// 敵
	std::vector<std::unique_ptr<Enemy>> m_Enemies;
};