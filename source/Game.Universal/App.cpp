#include "pch.h"
#include "App.h"
#include "GameMain.h"

using namespace DX;
using namespace std;
using namespace concurrency;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);
	return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new DirectXGame::App();
}

namespace DirectXGame
{
	App::App() :
		mWindowClosed(false),
		mWindowVisible(true)
	{
	}

	// The first method called when the IFrameworkView is being created.
	void App::Initialize(CoreApplicationView^ applicationView)
	{
		// Register event handlers for app lifecycle. This example includes Activated, so that we
		// can make the CoreWindow active and start rendering on the window.
		applicationView->Activated +=
			ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

		CoreApplication::Suspending +=
			ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

		CoreApplication::Resuming +=
			ref new EventHandler<Platform::Object^>(this, &App::OnResuming);

		// At this point we have access to the device. 
		// We can create the device-dependent resources.
		mDeviceResources = make_shared<DeviceResources>();
	}

	// Called when the CoreWindow object is created (or re-created).
	void App::SetWindow(CoreWindow^ window)
	{
		window->SizeChanged +=
			ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

		window->VisibilityChanged +=
			ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

		window->Closed +=
			ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

		DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

		currentDisplayInformation->DpiChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

		currentDisplayInformation->OrientationChanged +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

		DisplayInformation::DisplayContentsInvalidated +=
			ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

		mDeviceResources->SetWindow(window);
	}

	// Initializes scene resources, or loads a previously saved app state.
	void App::Load(Platform::String^ entryPoint)
	{
		if (mMain == nullptr)
		{
			mMain = unique_ptr<GameMain>(new GameMain(mDeviceResources));
		}
	}

	// This method is called after the window becomes active.
	void App::Run()
	{
		while (!mWindowClosed)
		{
			if (mWindowVisible)
			{
				CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

				mMain->Update();

				if (mMain->Render())
				{
					mDeviceResources->Present();
				}
			}
			else
			{
				CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
			}
		}

#if defined(DEBUG) || defined(_DEBUG)
		DumpD3DDebug();
#endif
	}

	// Required for IFrameworkView.
	// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
	// class is torn down while the app is in the foreground.
	void App::Uninitialize()
	{
	}

	// Application lifecycle event handlers.

	void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
	{
		// Run() won't start until the CoreWindow is activated.
		CoreWindow::GetForCurrentThread()->Activate();
	}

	void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
	{
		// Save app state asynchronously after requesting a deferral. Holding a deferral
		// indicates that the application is busy performing suspending operations. Be
		// aware that a deferral may not be held indefinitely. After about five seconds,
		// the app will be forced to exit.
		SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

		create_task([this, deferral]()
		{
			mDeviceResources->Trim();

			// Insert your code here.

			deferral->Complete();
		});
	}

	void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
	{
		// Restore any data or state that was unloaded on suspend. By default, data
		// and state are persisted when resuming from suspend. Note that this event
		// does not occur if the app was previously terminated.

		// Insert your code here.
	}

	// Window event handlers.

	void App::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args)
	{
		mDeviceResources->SetLogicalSize(Size(sender->Bounds.Width, sender->Bounds.Height));
		mMain->CreateWindowSizeDependentResources();
	}

	void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
	{
		mWindowVisible = args->Visible;
	}

	void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
	{
		mWindowClosed = true;
	}

	// DisplayInformation event handlers.

	void App::OnDpiChanged(DisplayInformation^ sender, Object^ args)
	{
		// Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
		// if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
		// you should always retrieve it using the GetDpi method.
		// See DeviceResources.cpp for more details.
		mDeviceResources->SetDpi(sender->LogicalDpi);
		mMain->CreateWindowSizeDependentResources();
	}

	void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
	{
		mDeviceResources->SetCurrentOrientation(sender->CurrentOrientation);
		mMain->CreateWindowSizeDependentResources();
	}

	void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
	{
		mDeviceResources->ValidateDevice();
	}
	
#if defined(DEBUG) || defined(_DEBUG)
	void App::DumpD3DDebug()
	{
		// Get the debug interface for the device.
		Microsoft::WRL::ComPtr<IDXGIDebug1> debugInterface = nullptr;
		ThrowIfFailed(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debugInterface)));

		// Dump the list of leaked objects to the debugger output window.
		ThrowIfFailed(debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL));
	}
#endif
}