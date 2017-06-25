#pragma once

#include "GameComponent.h"
#include "MatrixHelper.h"
#include <memory>
#include <vector>

namespace DX
{
	class KeyboardComponent;
	class GamePadComponent;
}

namespace DirectXGame
{
	class Snake;
	class SpriteManager;
	class SpawnManager;

	class SnakeManager final : public DX::GameComponent
	{
	public:
		SnakeManager(const std::shared_ptr<SpriteManager>& spriteManager, const std::shared_ptr<SpawnManager>& spawnManager,
			const std::shared_ptr<DX::KeyboardComponent>& keyboardComponent, const std::shared_ptr<DX::GamePadComponent>& gamePadComponent);
		
		void Update(const DX::StepTimer& timer) override;

	private:
		void CheckSpawnCollision();

		std::vector<std::shared_ptr<Snake>> mSnakes;
		std::shared_ptr<SpriteManager> mSpriteManager;
		std::shared_ptr<SpawnManager> mSpawnManager;

		std::shared_ptr<DX::KeyboardComponent> mKeyboardComponent;
		std::shared_ptr<DX::GamePadComponent> mGamePadComponent;

		
	};
}
