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
#include <Keyboard.h>

#include "StepTimer.h"
#include "DebugCamera.h"
#include "DebugText.h"
#include "FollowCamera.h"

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
class Game
{
public:

    Game();

	virtual ~Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();
    void Present();

    void CreateDevice();
    void CreateResources();

    void OnDeviceLost();

    // Device resources.
    HWND                                            m_window;
    int                                             m_outputWidth;
    int                                             m_outputHeight;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Microsoft::WRL::ComPtr<ID3D11Device>            m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice1;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     m_d3dContext;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext1;

    Microsoft::WRL::ComPtr<IDXGISwapChain>          m_swapChain;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain1;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

	// スプライトバッチ
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	// デバッグテキスト
	std::unique_ptr<DebugText> m_debugText;

	// コモンステート
	std::unique_ptr<DirectX::CommonStates> m_states;
	// ポリゴン表示用エフェクトファクトリ
	std::unique_ptr<DirectX::EffectFactory> m_effectFactory;
	// ポリゴン表示用エフェクト
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	// ポリゴン表示用入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	DirectX::SimpleMath::Vector3 m_pos[20];
	// 球のワールド行列
	DirectX::SimpleMath::Matrix m_world[20];
	float m_angle;
	float m_scale;
	float m_dfactor;

	std::unique_ptr<DirectX::Model> m_ModelSkydome;
	std::unique_ptr<DirectX::Model> m_ModelGround;
	std::unique_ptr<DirectX::Model> m_ModelBall;
	std::unique_ptr<DirectX::Model> m_ModelTeapot;

	std::unique_ptr<DirectX::Model> m_ModelHead;

	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	DirectX::SimpleMath::Vector3 m_TankPos;
	float	m_TankAngle;
	DirectX::SimpleMath::Matrix m_TankWorld;

	// Rendering loop timer.
    DX::StepTimer                                   m_timer;
	// デバッグ用カメラ
	DebugCamera*	m_DebugCamera;
	// 追従カメラ
	std::unique_ptr<FollowCamera> m_FollowCamera;
};