#pragma once

#include "GameComponent.h"
#include "MatrixHelper.h"
#include <memory>
#include <vector>

namespace DX
{
	class TextRenderer;
}

namespace DirectXGame
{
	class Snake;
	class SpriteManager;
	class SpawnManager;
	class InputComponent;
	class TimerComponent;

	class SnakeManager final : public DX::GameComponent
	{
	public:
		SnakeManager(const std::vector<std::shared_ptr<DX::TextRenderer>>& textRenderers, const std::shared_ptr<SpriteManager>& spriteManager,
			const std::shared_ptr<SpawnManager>& spawnManager, const std::shared_ptr<InputComponent>& gameCommands, const std::shared_ptr<TimerComponent>& timerComponent);
		
		void Update(const DX::StepTimer& timer) override;

	private:
		void GetPlayerHeading(std::uint32_t playerId, DirectX::XMFLOAT2 &headingOffset);
		void CheckSpawnCollision();
		void SnakeToSnakeCollision();
		void TestTimer(const std::shared_ptr<void>&);

		std::vector<std::shared_ptr<Snake>> mSnakes;
		std::shared_ptr<SpriteManager> mSpriteManager;
		std::shared_ptr<SpawnManager> mSpawnManager;
		std::shared_ptr<Snake> mWinner;

		std::vector<std::shared_ptr<DX::TextRenderer>> mScoreRenderers;

		std::shared_ptr<InputComponent> mInputComponent;
		std::shared_ptr<TimerComponent> mTimerComponent;
	};
}
