#include "pch.h"
#include "InputComponent.h"

using namespace DX;
using namespace std;
using namespace Windows::UI::Core;

#define KeyOf(playerId, command)  ((playerId << 16) | (static_cast<uint32_t>(command) & 0x0000FFFF))
#define PlayerIdFrom(key)  (key >> 16)
#define CommandFrom(key)  (key & 0x0000FFFF)

namespace DirectXGame
{
	const unordered_map<uint32_t, InputComponent::CommandInfo> InputComponent::CommandMapping =
	{
		// Game inputs
		{ KeyOf(0, Command::GameExit),{ KeyCheckType::WasPressedThisFrame, Keys::Escape, GamePadButtons::Back, MouseButtons::Middle } },

		// Player 1 inputs
		{ KeyOf(1, Command::StartRound),{ KeyCheckType::WasPressedThisFrame, Keys::PageUp, GamePadButtons::Start, MouseButtons::None } },
		{ KeyOf(1, Command::MoveUp),{ KeyCheckType::WasPressedThisFrame, Keys::Up, GamePadButtons::DPadUp, MouseButtons::None } },
		{ KeyOf(1, Command::MoveLeft), { KeyCheckType::WasPressedThisFrame, Keys::Left, GamePadButtons::DPadLeft, MouseButtons::None} },
		{ KeyOf(1, Command::MoveDown),{ KeyCheckType::WasPressedThisFrame, Keys::Down, GamePadButtons::DPadDown, MouseButtons::None } },
		{ KeyOf(1, Command::MoveRight), { KeyCheckType::WasPressedThisFrame, Keys::Right, GamePadButtons::DPadRight, MouseButtons::None } },

		// Player 2 inputs
		{ KeyOf(2, Command::StartRound),{ KeyCheckType::WasPressedThisFrame, Keys::Q, GamePadButtons::Start, MouseButtons::None } },
		{ KeyOf(2, Command::MoveUp),{ KeyCheckType::WasPressedThisFrame, Keys::W, GamePadButtons::DPadUp, MouseButtons::None } },
		{ KeyOf(2, Command::MoveLeft),{ KeyCheckType::WasPressedThisFrame, Keys::A, GamePadButtons::DPadLeft, MouseButtons::None } },
		{ KeyOf(2, Command::MoveDown),{ KeyCheckType::WasPressedThisFrame, Keys::S, GamePadButtons::DPadDown, MouseButtons::None } },
		{ KeyOf(2, Command::MoveRight),{ KeyCheckType::WasPressedThisFrame, Keys::D, GamePadButtons::DPadRight, MouseButtons::None } },

		// Debug inputs
		{ KeyOf(0, Command::MasterDebugToggle), { KeyCheckType::WasPressedThisFrame, Keys::OemTilde, GamePadButtons::None, MouseButtons::None } },
		{ KeyOf(0, Command::DebugPause), { KeyCheckType::WasPressedThisFrame, Keys::Space, GamePadButtons::None, MouseButtons::None } },
		{ KeyOf(0, Command::DebugStepForward), { KeyCheckType::WasPressedThisFrame, Keys::F10, GamePadButtons::None, MouseButtons::None } },
		{ KeyOf(1, Command::DebugAddBodyBlock), { KeyCheckType::WasPressedThisFrame, Keys::OemPipe, GamePadButtons::None, MouseButtons::None } },
		{ KeyOf(1, Command::DebugShrink), { KeyCheckType::WasPressedThisFrame, Keys::OemMinus, GamePadButtons::None, MouseButtons::None } },
		{ KeyOf(1, Command::DebugExpand), { KeyCheckType::WasPressedThisFrame, Keys::OemPlus, GamePadButtons::None, MouseButtons::None } }
	};

	const unordered_map<InputComponent::KeyCheckType, function<bool(const shared_ptr<KeyboardComponent>&, Keys)>> InputComponent::KeyCheckMethods =
	{
		{ KeyCheckType::IsUp, [] (const shared_ptr<KeyboardComponent>& keyboard, DX::Keys key) { return keyboard->IsKeyUp(key); } },
		{ KeyCheckType::IsDown, [] (const shared_ptr<KeyboardComponent>& keyboard, DX::Keys key) { return keyboard->IsKeyDown(key); } },
		{ KeyCheckType::WasUp, [] (const shared_ptr<KeyboardComponent>& keyboard, DX::Keys key) { return keyboard->WasKeyUp(key); } },
		{ KeyCheckType::WasDown, [] (const shared_ptr<KeyboardComponent>& keyboard, DX::Keys key) { return keyboard->WasKeyDown(key); } },
		{ KeyCheckType::WasPressedThisFrame, [] (const shared_ptr<KeyboardComponent>& keyboard, DX::Keys key) { return keyboard->WasKeyPressedThisFrame(key); } },
		{ KeyCheckType::WasReleasedThisFrame, [] (const shared_ptr<KeyboardComponent>& keyboard, DX::Keys key) { return keyboard->WasKeyReleasedThisFrame(key); } },
		{ KeyCheckType::IsHeldDown, [] (const shared_ptr<KeyboardComponent>& keyboard, DX::Keys key) { return keyboard->IsKeyHeldDown(key); } }
	};

	const unordered_map<InputComponent::KeyCheckType, function<bool(const shared_ptr<GamePadComponent>&, GamePadButtons)>> InputComponent::PadCheckMethods =
	{
		{ KeyCheckType::IsUp, [](const shared_ptr<GamePadComponent>& gamePad, DX::GamePadButtons button) { return gamePad->IsButtonUp(button); } },
		{ KeyCheckType::IsDown, [](const shared_ptr<GamePadComponent>& gamePad, DX::GamePadButtons button) { return gamePad->IsButtonDown(button); } },
		{ KeyCheckType::WasUp, [](const shared_ptr<GamePadComponent>& gamePad, DX::GamePadButtons button) { return gamePad->WasButtonUp(button); } },
		{ KeyCheckType::WasDown, [](const shared_ptr<GamePadComponent>& gamePad, DX::GamePadButtons button) { return gamePad->WasButtonDown(button); } },
		{ KeyCheckType::WasPressedThisFrame, [](const shared_ptr<GamePadComponent>& gamePad, DX::GamePadButtons button) { return gamePad->WasButtonPressedThisFrame(button); } },
		{ KeyCheckType::WasReleasedThisFrame, [](const shared_ptr<GamePadComponent>& gamePad, DX::GamePadButtons button) { return gamePad->WasButtonReleasedThisFrame(button); } },
		{ KeyCheckType::IsHeldDown, [](const shared_ptr<GamePadComponent>& gamePad, DX::GamePadButtons button) { return gamePad->IsButtonHeldDown(button); } }
	};

	const unordered_map<InputComponent::KeyCheckType, function<bool(const shared_ptr<MouseComponent>&, MouseButtons)>> InputComponent::MouseCheckMethods =
	{
		{ KeyCheckType::IsUp, [](const shared_ptr<MouseComponent>& mouse, DX::MouseButtons button) { return mouse->IsButtonUp(button); } },
		{ KeyCheckType::IsDown, [](const shared_ptr<MouseComponent>& mouse, DX::MouseButtons button) { return mouse->IsButtonDown(button); } },
		{ KeyCheckType::WasUp, [](const shared_ptr<MouseComponent>& mouse, DX::MouseButtons button) { return mouse->WasButtonUp(button); } },
		{ KeyCheckType::WasDown, [](const shared_ptr<MouseComponent>& mouse, DX::MouseButtons button) { return mouse->WasButtonDown(button); } },
		{ KeyCheckType::WasPressedThisFrame, [](const shared_ptr<MouseComponent>& mouse, DX::MouseButtons button) { return mouse->WasButtonPressedThisFrame(button); } },
		{ KeyCheckType::WasReleasedThisFrame, [](const shared_ptr<MouseComponent>& mouse, DX::MouseButtons button) { return mouse->WasButtonReleasedThisFrame(button); } },
		{ KeyCheckType::IsHeldDown, [](const shared_ptr<MouseComponent>& mouse, DX::MouseButtons button) { return mouse->IsButtonHeldDown(button); } }
	};

	InputComponent::InputComponent(const shared_ptr<DX::DeviceResources>& deviceResources, CoreWindow^ window) :
		GameComponent(deviceResources),
		mKeyboard(make_shared<KeyboardComponent>(mDeviceResources)), mMouse(make_shared<MouseComponent>(mDeviceResources))
	{
		mKeyboard->Keyboard()->SetWindow(window);
		mMouse->Mouse()->SetWindow(window);

		mGamePads.reserve(ProgramHelper::PlayerConfigs.size());
		for (std::uint32_t index = 0; index < ProgramHelper::PlayerConfigs.size(); ++index)
		{
			mGamePads.push_back(make_shared<GamePadComponent>(mDeviceResources, index));
		}
	}

	void InputComponent::Update(const DX::StepTimer& gameTime)
	{
		mKeyboard->Update(gameTime);
		for (auto& gamepad : mGamePads)
		{
			gamepad->Update(gameTime);
		}
		mMouse->Update(gameTime);
	}

	bool InputComponent::IsCommandGiven(std::uint32_t playerId, Command command)
	{
		uint32_t key = KeyOf(playerId, command);
		if (CommandMapping.find(key) == CommandMapping.end())
		{
			return false;
		}

		const auto& info = CommandMapping.at(key);
		bool result = false;
		if (info.mKey != Keys::None)
		{
			result |= KeyCheckMethods.at(info.mType)(mKeyboard, info.mKey);
		}

		if (info.mPadButton != GamePadButtons::None)
		{
			if (playerId == 0)
			{
				for (std::uint32_t index = 0; index < ProgramHelper::PlayerConfigs.size(); ++index)
				{
					result |= PadCheckMethods.at(info.mType)(mGamePads[index], info.mPadButton);
				}
			}
			else
			{
				result |= PadCheckMethods.at(info.mType)(mGamePads[playerId - 1], info.mPadButton);
			}
		}

		if (info.mMouseButton != MouseButtons::None)
		{
			result |= MouseCheckMethods.at(info.mType)(mMouse, info.mMouseButton);
		}
		return result;
	}

	void InputComponent::VibrateController(std::uint32_t playerId, float timePeriod, float leftMotor, float rightMotor, float leftTrigger, float rightTrigger)
	{
		assert(playerId >= 0 && playerId <= ProgramHelper::PlayerConfigs.size());
		if (playerId > 0)
		{
			mGamePads[playerId - 1]->VibrateController(timePeriod, leftMotor, rightMotor, leftTrigger, rightTrigger);
		}
		else
		{
			for (auto& gamePad : mGamePads)
			{
				gamePad->VibrateController(timePeriod, leftMotor, rightMotor, leftTrigger, rightTrigger);
			}
		}
	}

	void InputComponent::StopControllerVibration(std::uint32_t playerId)
	{
		assert(playerId >= 0 && playerId <= ProgramHelper::PlayerConfigs.size());
		if (playerId > 0)
		{
			mGamePads[playerId - 1]->StopVibration();
		}
		else
		{
			for (auto& gamePad : mGamePads)
			{
				gamePad->StopVibration();
			}
		}
	}
}
