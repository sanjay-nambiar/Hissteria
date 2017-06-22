#include "pch.h"
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
		mDeviceResources(deviceResources)
	{
		// Register to be notified if the Device is lost or recreated
		mDeviceResources->RegisterDeviceNotify(this);

		auto camera = make_shared<OrthographicCamera>(mDeviceResources);
		mComponents.push_back(camera);
		camera->SetPosition(0, 0, 1);

		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		mKeyboard = make_shared<KeyboardComponent>(mDeviceResources);		
		mKeyboard->Keyboard()->SetWindow(window);
		mComponents.push_back(mKeyboard);

		mMouse = make_shared<MouseComponent>(mDeviceResources);		
		mMouse->Mouse()->SetWindow(window);
		mComponents.push_back(mMouse);

		mGamePad = make_shared<GamePadComponent>(mDeviceResources);
		mComponents.push_back(mGamePad);

		auto fpsTextRenderer = make_shared<FpsTextRenderer>(mDeviceResources);
		mComponents.push_back(fpsTextRenderer);

		auto spawnManager = make_shared<SpawnManager>(mDeviceResources, camera);
		mComponents.push_back(spawnManager);

		/*auto fieldManager = make_shared<FieldManager>(mDeviceResources, camera);
		mComponents.push_back(fieldManager);

		auto ballManager = make_shared<BallManager>(mDeviceResources, camera);
		ballManager->SetActiveField(fieldManager->ActiveField());
		mComponents.push_back(ballManager);		

		const int32_t spriteRowCount = 12;
		const int32_t spriteColumnCount = 15;
		auto spriteDemoManager = make_shared<SpriteDemoManager>(mDeviceResources, camera, spriteRowCount, spriteColumnCount);		
		const XMFLOAT2 center((-spriteColumnCount + 1) * SpriteDemoManager::SpriteScale.x, (-spriteRowCount + 1) * SpriteDemoManager::SpriteScale.y);
		spriteDemoManager->SetPositon(center);
		mComponents.push_back(spriteDemoManager);*/

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
			for (auto& component : mComponents)
			{
				component->Update(mTimer);
			}

			if (mKeyboard->WasKeyPressedThisFrame(Keys::Escape) ||
				mMouse->WasButtonPressedThisFrame(MouseButtons::Middle) ||
				mGamePad->WasButtonPressedThisFrame(GamePadButtons::Back))
			{
				CoreApplication::Exit();
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