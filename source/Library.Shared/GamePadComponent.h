#pragma once

#include "GameComponent.h"
#include <GamePad.h>

namespace DX
{
	enum class GamePadButtons
	{
		A,
		B,
		X,
		Y,
		LeftStick,
		RightStick,
		LeftShoulder,
		RightShoulder,
		Back,
		Start,
		DPadUp,
		DPadDown,
		DPadLeft,
		DPadRight,
		None
	};

	class GamePadComponent final : public GameComponent
	{
	public:
		static DirectX::GamePad* GamePad();
		
		GamePadComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources, int player = 0);
		GamePadComponent(const GamePadComponent&) = default;
		GamePadComponent& operator=(const GamePadComponent&) = default;
		GamePadComponent(GamePadComponent&&) = default;
		GamePadComponent& operator=(GamePadComponent&&) = default;
		~GamePadComponent() = default;

		int Player() const;
		const DirectX::GamePad::State& CurrentState() const;
		const DirectX::GamePad::State& LastState() const;

		virtual void Update(const StepTimer& gameTime) override;

		bool IsButtonUp(GamePadButtons button) const;
		bool IsButtonDown(GamePadButtons button) const;
		bool WasButtonUp(GamePadButtons button) const;
		bool WasButtonDown(GamePadButtons button) const;
		bool WasButtonPressedThisFrame(GamePadButtons button) const;
		bool WasButtonReleasedThisFrame(GamePadButtons button) const;
		bool IsButtonHeldDown(GamePadButtons button) const;

	private:
		static std::unique_ptr<DirectX::GamePad> sGamePad;

		bool GetButtonState(const DirectX::GamePad::State& state, GamePadButtons button) const;

		int mPlayer;
		DirectX::GamePad::State mCurrentState;
		DirectX::GamePad::State mLastState;		
	};
}