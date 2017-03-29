//
// Game.h
//

#pragma once

#include "StepTimer.h"
#include "VertexTypes.h"
#include "PrimitiveBatch.h"
#include "SimpleMath.h"
#include "Effects.h"
#include "CommonStates.h"

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game
{
public:

    Game();

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

	void PolygonInit();

	void PolygonTerm();

	void PolygonDraw(DirectX::SimpleMath::Matrix view, DirectX::SimpleMath::Matrix proj);

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

	// コモンステート
	std::unique_ptr <DirectX::CommonStates> commonState;	
	// プリミティブバッチ
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormal>> primitiveBatch;
	// ポリゴン表示用エフェクト
	std::unique_ptr<DirectX::BasicEffect>	polygonEffect;
	// ポリゴン表示用入力レイアウト
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	polygonInputLayout;

	// Rendering loop timer.
    DX::StepTimer                                   m_timer;
};