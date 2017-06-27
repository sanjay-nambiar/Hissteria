#include "pch.h"
#include "GamePadComponent.h"
#include "StepTimer.h"

using namespace std;
using namespace DirectX;

namespace DX
{
	unique_ptr<GamePad> GamePadComponent::sGamePad(new DirectX::GamePad);

	GamePad* GamePadComponent::GamePad()
	{
		return sGamePad.get();
	}

	GamePadComponent::GamePadComponent(const shared_ptr<DX::DeviceResources>& deviceResources, int player) :
		GameComponent(deviceResources), mPlayer(player), mVibrationPeriod(0.0f), mElapsedVibrationTime(0.0f)
	{
		mCurrentState = sGamePad->GetState(mPlayer);
		mLastState = mCurrentState;
	}

	int GamePadComponent::Player() const
	{
		return mPlayer;
	}

	const GamePad::State& GamePadComponent::CurrentState() const
	{
		return mCurrentState;
	}

	const GamePad::State& GamePadComponent::LastState() const
	{
		return mLastState;
	}

	void GamePadComponent::Update(const StepTimer& timer)
	{
		mElapsedVibrationTime += static_cast<float>(timer.GetElapsedSeconds());
		if (mElapsedVibrationTime >= mVibrationPeriod)
		{
			StopVibration();
		};

		mLastState = mCurrentState;
		mCurrentState = sGamePad->GetState(mPlayer);
	}

	bool GamePadComponent::IsButtonUp(GamePadButtons button) const
	{
		return GetButtonState(mCurrentState, button) == false;
	}

	bool GamePadComponent::IsButtonDown(GamePadButtons button) const
	{
		return GetButtonState(mCurrentState, button);
	}

	bool GamePadComponent::WasButtonUp(GamePadButtons button) const
	{
		return GetButtonState(mLastState, button) == false;
	}

	bool GamePadComponent::WasButtonDown(GamePadButtons button) const
	{
		return GetButtonState(mLastState, button);
	}

	bool GamePadComponent::WasButtonPressedThisFrame(GamePadButtons button) const
	{
		return (IsButtonDown(button) && WasButtonUp(button));
	}

	bool GamePadComponent::WasButtonReleasedThisFrame(GamePadButtons button) const
	{
		return (IsButtonUp(button) && WasButtonDown(button));
	}

	bool GamePadComponent::IsButtonHeldDown(GamePadButtons button) const
	{
		return (IsButtonDown(button) && WasButtonDown(button));
	}

	bool GamePadComponent::GetButtonState(const GamePad::State& state, GamePadButtons button) const
	{
		switch (button)
		{
			case GamePadButtons::A:
				return state.buttons.a;

			case GamePadButtons::B:
				return state.buttons.b;

			case GamePadButtons::X:
				return state.buttons.x;

			case GamePadButtons::Y:
				return state.buttons.y;

			case GamePadButtons::LeftStick:
				return state.buttons.leftStick;

			case GamePadButtons::RightStick:
				return state.buttons.rightStick;

			case GamePadButtons::LeftShoulder:
				return state.buttons.leftShoulder;

			case GamePadButtons::RightShoulder:
				return state.buttons.rightShoulder;

			case GamePadButtons::Back:
				return state.buttons.back;

			case GamePadButtons::Start:
				return state.buttons.start;

			case GamePadButtons::DPadUp:
				return state.dpad.up;

			case GamePadButtons::DPadDown:
				return state.dpad.down;

			case GamePadButtons::DPadLeft:
				return state.dpad.left;

			case GamePadButtons::DPadRight:
				return state.dpad.right;

			default:
				throw exception("Invalid GamePadButtons.");
		}
	}

	void GamePadComponent::VibrateController(float timePeriod, float leftMotor, float rightMotor, float leftTrigger, float rightTrigger)
	{
		mVibrationPeriod = timePeriod;
		mElapsedVibrationTime = 0.0f;
		GamePad()->SetVibration(mPlayer, leftMotor, rightMotor, leftTrigger, rightTrigger);
	}

	void GamePadComponent::StopVibration()
	{
		mVibrationPeriod = 0.0f;
		mElapsedVibrationTime = 0.0f;
		GamePad()->SetVibration(mPlayer, 0.0f, 0.0f);
	}
}