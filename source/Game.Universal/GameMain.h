#pragma once

#include "StepTimer.h"
#include "DeviceResources.h"
#include <vector>
#include <memory>

namespace DX
{
	class GameComponent;
	class MouseComponent;
	class KeyboardComponent;
	class GamePadComponent;
	class GameCommands;
	class FpsTextRenderer;
}

// Renders Direct2D and 3D content on the screen.
namespace DirectXGame
{
	class GameMain : public DX::IDeviceNotify
	{
	public:
		GameMain(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~GameMain();
		void CreateWindowSizeDependentResources();
		void Update();
		bool Render();

		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();

	private:
		enum class GameState
		{
			Menu,
			InGame,
			Pause,
			DebugPause
		};

		void IntializeResources();

		std::shared_ptr<DX::DeviceResources> mDeviceResources;
		std::vector<std::shared_ptr<DX::GameComponent>> mComponents;
		DX::StepTimer mTimer;
		std::shared_ptr<InputComponent> mInputComponent;
		std::shared_ptr<DX::FpsTextRenderer> mFpsTextRenderer;
		std::shared_ptr<DX::TextRenderer> mTextRenderer;

		GameState mGameState;
		GameState mPreviousGameState;
		bool mDebugStep;
	};
}