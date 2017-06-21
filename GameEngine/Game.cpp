//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "ModelEffect.h"
#include <fstream>
#include <sstream>
#include <algorithm>

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()
{
	m_deviceResources = DX::DeviceResources::GetInstance();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	{
		// デバッグカメラを生成
		m_DebugCamera = std::make_unique<DebugCamera>(width, height);

		// 追従カメラを生成
		m_FollowCamera = std::make_unique<FollowCamera>();

		// 追従カメラを有効に
		m_CurrentCamera = m_FollowCamera.get();
	}

	// Obj3Dの静的な初期化
	{
		Obj3D::Defs defs;
		defs.pDevice = m_deviceResources->GetD3DDevice();
		defs.pDeviceContext = m_deviceResources->GetD3DDeviceContext();
		defs.pCamera = m_FollowCamera.get();

		Obj3D::StaticInitialize(defs);
	}

	// LandShapeの静的な初期化
	LandShapeCommonDef def;
	def.pCamera = m_FollowCamera.get();
	def.pDevice = m_deviceResources->GetD3DDevice();
	def.pDeviceContext = m_deviceResources->GetD3DDeviceContext();
	LandShape::InitializeCommon(def);

	m_keyboard = std::make_unique<Keyboard>();

	// プレイヤー作成
	m_Player = std::make_unique<Player>(m_keyboard.get());

	m_Enemies.resize(ENEMY_NUM);
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		m_Enemies[i] = std::make_unique<Enemy>();
		m_Enemies[i]->Initialize();
	}
	
	// モデルをファイルからロード
	m_ObjSkydome = std::make_unique<Obj3D>();
	m_ObjSkydome->DisableLighting();

	{// 地形ロード
		wifstream ifs("CSV/LandShape.csv");

		wstring line;

		// 1行目をスキップ
		getline(ifs, line);

		// 各行のデータを解析
		while (getline(ifs, line)) {

			wstring filename_landshape;
			wstring filename_cmo;			

			// 1行分を文字列ストリームに変換
			wstringstream stream(line);

			// 区切り記号
			wchar_t delim = L',';

			// ファイル名の文字列を読み込み
			getline(stream, filename_landshape, delim);

			// ファイル名の文字列を読み込み
			getline(stream, filename_cmo, delim);

			std::unique_ptr<LandShape> landshape = std::make_unique<LandShape>();
			// 読み込む
			landshape->Initialize(filename_landshape.c_str(), filename_cmo.c_str());

			getline(stream, line);

			// カンマを空白に置き換え
			std::replace(line.begin(), line.end(), L',', L' ');

			// 残りを文字列ストリームに再変換
			stream.str(L"");
			stream.clear(wstringstream::goodbit);
			stream << line;

			// スケーリングの読み取り
			float scale;
			stream >> scale;
			landshape->SetScale(scale);

			// 角度の読み取り
			Vector3 rotation;
			stream >> rotation.z;
			stream >> rotation.x;
			stream >> rotation.y;
			landshape->SetRot(rotation);

			// 座標の読み取り
			Vector3 position;
			stream >> position.x;
			stream >> position.y;
			stream >> position.z;
			landshape->SetTrans(position);

			// ライト無効
			int light_cut;
			stream >> light_cut;

			if (light_cut)
			{
				landshape->DisableLighting();
			}

			// コンテナに追加
			m_pLandShapeArray.push_back(std::move(landshape));
		}
	}
	m_Player->SetLandShapeArray(&m_pLandShapeArray);

	m_ObjSkydome->LoadModelFile(L"Resources/skydome.cmo");

	m_spriteBatch = std::make_unique<SpriteBatch>(m_deviceResources->GetD3DDeviceContext());
	m_debugText = std::make_unique<DebugText>(m_deviceResources->GetD3DDevice(), m_spriteBatch.get());

	// 指定範囲をランダムで返すラムダ式
	auto rand_value = [](float min, float max)
	{
		return (max - min) * (float)rand() / RAND_MAX + min;
	};

}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

	float fps = m_timer.GetFramesPerSecond();

	m_debugText->AddText(Vector2(0, 0), L"FPS:%d", m_timer.GetFramesPerSecond());

	for (std::vector<std::unique_ptr<Enemy>>::iterator it = m_Enemies.begin();
		it != m_Enemies.end();
		it++)
	{
		(*it)->Update();
	}
	
	for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray.begin();
		it != m_pLandShapeArray.end();
		it++)
	{
		(*it)->Update();
	}

	m_Player->Update();

	// 攻撃当たり判定
	{
		// プレイヤーの攻撃当たり球
		const Sphere& sphereA = m_Player->GetCollisionNodeBullet();

		// 全ての敵について判定する
		for (std::vector<std::unique_ptr<Enemy>>::iterator it = m_Enemies.begin();
			it != m_Enemies.end();
			)
		{
			Enemy* enemy = it->get();

			// 敵の被攻撃当たり球
			const Sphere& sphereB = enemy->GetCollisionNodeBody();

			Vector3 inter;

			// 球と球の当たり
			if (CheckSphere2Sphere(sphereA, sphereB, &inter))
			{
				m_Player->ResetBullet();

				ModelEffectManager::getInstance()->Entry(
					L"Resources/HitEffect.cmo",
					10,
					inter,	// 座標
					Vector3(0, 0, 0),	// 速度
					Vector3(0, 0, 0),	// 加速度
					Vector3(0, 0, 0),	// 回転角（初期）
					Vector3(0, 0, 0),	// 回転角（最終）
					Vector3(0, 0, 0),	// スケール（初期）
					Vector3(6, 6, 6)	// スケール（最終）
				);
				it = m_Enemies.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	Keyboard::State keystate = m_keyboard->GetState();
	m_keyboardTracker.Update(keystate);

	if (m_keyboardTracker.IsKeyPressed(Keyboard::D1))
	{
		CollisionNode::SetDebugVisible(!CollisionNode::GetDebugVisible());
	}

	// カメラ更新
	m_DebugCamera->Update();

	// 追従カメラ
	m_FollowCamera->SetTarget(m_Player.get());
	m_FollowCamera->Update();

	ModelEffectManager::getInstance()->Update();
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();
    m_deviceResources->PIXBeginEvent(L"Render");
	
    // TODO: Add your rendering code here.
	m_ObjSkydome->Draw();
	for (std::vector<std::unique_ptr<LandShape>>::iterator it = m_pLandShapeArray.begin();
		it != m_pLandShapeArray.end();
		it++)
	{
		(*it)->Draw();
	}
	auto context = m_deviceResources->GetD3DDeviceContext();

	m_Player->Draw();

	for (std::vector<std::unique_ptr<Enemy>>::iterator it = m_Enemies.begin();
		it != m_Enemies.end();
		it++)
	{
		(*it)->Draw();
	}

	ModelEffectManager::getInstance()->Draw();
    m_deviceResources->PIXEndEvent();

	m_spriteBatch->Begin();
	m_debugText->Draw();
	m_spriteBatch->End();
    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
	m_deviceResources->PIXBeginEvent(L"Clear");

	// Clear the views.
	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);

	// Set the viewport.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
