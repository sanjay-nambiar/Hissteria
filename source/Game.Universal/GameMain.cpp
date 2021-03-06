﻿#include "pch.h"
#include "GameMain.h"

using namespace DX;
using namespace std;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;
using namespace Concurrency;

namespace DirectXGame
{
	// Loads and initializes application assets when the application is loaded.
	GameMain::GameMain(const shared_ptr<DX::DeviceResources>& deviceResources) :
		mDeviceResources(deviceResources), mGameState(GameState::InGame), mPreviousGameState(GameState::Menu), mDebugStep(false)
	{
		// Register to be notified if the Device is lost or recreated
		mDeviceResources->RegisterDeviceNotify(this);

		auto camera = make_shared<OrthographicCamera>(mDeviceResources);
		mComponents.push_back(camera);
		camera->SetPosition(0, 0, 1);

		// Input component is not pushed to components list since it is updated manually so that debug inputs can be detected during pause game
		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		mInputComponent = make_shared<InputComponent>(deviceResources, window);

		auto timerComponent = make_shared<TimerComponent>(deviceResources);
		mComponents.push_back(timerComponent);

		mFpsTextRenderer = make_shared<FpsTextRenderer>(mDeviceResources);
		mFpsTextRenderer->SetVisible(ProgramHelper::IsDebugEnabled);
		mComponents.push_back(mFpsTextRenderer);

		for (const auto& config : ProgramHelper::PlayerConfigs)
		{
			mTextRenderers.push_back(make_shared<TextRenderer>(mDeviceResources));
			mComponents.push_back(mTextRenderers.back());
			mTextRenderers.back()->SetTextFormatting(config.mHeadColor, config.mScoreAnchorPoint);
		}
		mTextRenderers.push_back(make_shared<TextRenderer>(mDeviceResources));
		mComponents.push_back(mTextRenderers.back());
		mTextRenderers.back()->SetTextFormatting(ColorHelper::White(), TextRenderer::AnchorPoint::Top);

		mTextRenderers.push_back(make_shared<TextRenderer>(mDeviceResources));
		mComponents.push_back(mTextRenderers.back());
		mTextRenderers.back()->SetFont(TextRenderer::DefaultFont, 72.0f);
		mTextRenderers.back()->SetTextFormatting(ColorHelper::White(), TextRenderer::AnchorPoint::Center);

		auto spriteManager = make_shared<SpriteManager>(mDeviceResources, camera);
		mComponents.push_back(spriteManager);

		auto spawnManager = make_shared<SpawnManager>(1, spriteManager);
		mComponents.push_back(spawnManager);

		auto snakeManager = make_shared<SnakeManager>(mTextRenderers, spriteManager, spawnManager, mInputComponent, timerComponent);
		mComponents.push_back(snakeManager);

		mTimer.SetFixedTimeStep(true);
		mTimer.SetTargetElapsedSeconds(1.0 / 60);

		IntializeResources();
	}

	GameMain::~GameMain()
	{
		mDeviceResources->RegisterDeviceNotify(nullptr);
	}

	// Updates application state when the window size changes (e.g. device orientation change)
	void GameMain::CreateWindowSizeDependentResources()
	{
		for (auto& component : mComponents)
		{
			component->CreateWindowSizeDependentResources();
		}
	}

	// Updates the application state once per frame.
	void GameMain::Update()
	{
		// Update scene objects.
		mTimer.Tick([&]()
		{
			mInputComponent->Update(mTimer);

			if (mGameState != GameState::DebugPause || (mGameState == GameState::DebugPause && mDebugStep))
			{
				for (auto& component : mComponents)
				{
					component->Update(mTimer);
				}
				mDebugStep = false;
			}

			if (mInputComponent->IsCommandGiven(0, InputComponent::Command::GameExit))
			{
				CoreApplication::Exit();
			}


			// Turn on master debug
			if (mInputComponent->IsCommandGiven(0, InputComponent::Command::MasterDebugToggle))
			{
				ProgramHelper::IsDebugEnabled = !ProgramHelper::IsDebugEnabled;
				mFpsTextRenderer->SetVisible(ProgramHelper::IsDebugEnabled);
			}

			// Debug keys
			if (ProgramHelper::IsDebugEnabled)
			{
				if (mInputComponent->IsCommandGiven(0, InputComponent::Command::DebugPause))
				{
					if (mGameState != GameState::DebugPause)
					{
						mPreviousGameState = mGameState;
						mGameState = GameState::DebugPause;
					}
					else
					{
						GameState temp = mGameState;
						mGameState = mPreviousGameState;
						mPreviousGameState = temp;
					}
				}
			}

			if (mInputComponent->IsCommandGiven(0, InputComponent::Command::DebugStepForward))
			{
				mDebugStep = true;
			}
		});
	}

	// Renders the current frame according to the current application state.
	// Returns true if the frame was rendered and is ready to be displayed.
	bool GameMain::Render()
	{
		// Don't try to render anything before the first Update.
		if (mTimer.GetFrameCount() == 0)
		{
			return false;
		}

		auto context = mDeviceResources->GetD3DDeviceContext();

		// Reset the viewport to target the whole screen.
		auto viewport = mDeviceResources->GetScreenViewport();
		context->RSSetViewports(1, &viewport);

		// Reset render targets to the screen.
		ID3D11RenderTargetView *const targets[1] = { mDeviceResources->GetBackBufferRenderTargetView() };
		context->OMSetRenderTargets(1, targets, mDeviceResources->GetDepthStencilView());

		// Clear the back buffer and depth stencil view.
		context->ClearRenderTargetView(mDeviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::Black);
		context->ClearDepthStencilView(mDeviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		for (auto& component : mComponents)
		{
			auto drawableComponent = dynamic_pointer_cast<DrawableGameComponent>(component);
			if (drawableComponent != nullptr && drawableComponent->Visible())
			{
				drawableComponent->Render(mTimer);
			}
		}

		return true;
	}

	// Notifies renderers that device resources need to be released.
	void GameMain::OnDeviceLost()
	{
		for (auto& component : mComponents)
		{
			component->ReleaseDeviceDependentResources();
		}
	}

	// Notifies renderers that device resources may now be recreated.
	void GameMain::OnDeviceRestored()
	{
		IntializeResources();
	}

	void GameMain::IntializeResources()
	{
		for (auto& component : mComponents)
		{
			component->CreateDeviceDependentResources();
		}

		CreateWindowSizeDependentResources();
	}
}