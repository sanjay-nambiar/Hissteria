#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include "InputComponent.h"


namespace Windows
{
	namespace UI
	{
		namespace Core
		{
			ref class CoreWindow;
		}
	}
}

namespace DX
{
	class StepTimer;
	class KeyboardComponent;
	class GamePadComponent;
	class MouseComponent;
	class DeviceResources;
}

namespace DirectXGame
{
	class InputComponent final : public DX::GameComponent
	{
	public:
		enum class Command
		{
			GameExit,

			MoveLeft,
			MoveRight,
			MoveUp,
			MoveDown,

			MasterDebugToggle,
			DebugPause,
			DebugStepForward,
			DebugAddBodyBlock,
			DebugShrink,
			DebugExpand
		};

		InputComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources, Windows::UI::Core::CoreWindow^ window);
		InputComponent(const InputComponent&) = default;
		InputComponent& operator=(const InputComponent&) = default;
		InputComponent(InputComponent&&) = default;
		InputComponent& operator=(InputComponent&&) = default;
		~InputComponent() = default;

		void Update(const DX::StepTimer& gameTime) override;
		bool IsCommandGiven(std::uint32_t playerId, Command command);
		void SetVibration(float left, float right);
	private:

		enum class KeyCheckType
		{
			IsUp,
			IsDown,
			WasUp,
			WasDown,
			WasPressedThisFrame,
			WasReleasedThisFrame,
			IsHeldDown
		};

		struct CommandInfo
		{
			KeyCheckType mType;
			DX::Keys mKey;
			DX::GamePadButtons mPadButton;
			DX::MouseButtons mMouseButton;
		};

		std::shared_ptr<DX::KeyboardComponent> mKeyboard;
		std::shared_ptr<DX::GamePadComponent> mGamePad;
		std::shared_ptr<DX::MouseComponent> mMouse;

		static const std::unordered_map<std::uint32_t, CommandInfo> CommandMapping;
		static const std::unordered_map < KeyCheckType, std::function<bool(const std::shared_ptr<DX::KeyboardComponent>&, DX::Keys)>> KeyCheckMethods;
		static const std::unordered_map<KeyCheckType, std::function<bool(const std::shared_ptr<DX::GamePadComponent>&, DX::GamePadButtons)>> PadCheckMethods;
		static const std::unordered_map<KeyCheckType, std::function<bool(const std::shared_ptr<DX::MouseComponent>&, DX::MouseButtons)>> MouseCheckMethods;
	};
}
