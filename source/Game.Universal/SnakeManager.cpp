#include "pch.h"
#include "SnakeManager.h"
#include "Snake.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	SnakeManager::SnakeManager(const std::shared_ptr<SpriteManager>& spriteManager, const std::shared_ptr<SpawnManager>& spawnManager,
		const std::shared_ptr<KeyboardComponent>& keyboardComponent, const std::shared_ptr<GamePadComponent>& gamePadComponent) :
		GameComponent(nullptr),
		mSpriteManager(spriteManager), mSpawnManager(spawnManager),
		mKeyboardComponent(keyboardComponent), mGamePadComponent(gamePadComponent)
	{
		XMFLOAT2 dimension = { 6.0f, 3.0f };
		XMFLOAT2 heading = { 1.0f, 0.0f };
		auto snake = make_shared<Snake>(Snake::SnakeType::ChainLink, 10, dimension, heading, mSpriteManager);
		mSnakes.push_back(snake);
	}

	void SnakeManager::Update(const DX::StepTimer& timer)
	{
		XMFLOAT2 headingOffset = { 0.0f, 0.0f };
		if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Up))
		{
			headingOffset.y = 1.0f;
		}
		else if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Down))
		{
			headingOffset.y = -1.0f;
		}
		else if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Right))
		{
			headingOffset.x = 1.0f;
		}
		else if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::Left))
		{
			headingOffset.x = -1.0f;
		}

		//const auto& mSpawns = mSpawnManager->Spawns();
		for (auto& snake : mSnakes)
		{
			if ((headingOffset.x + headingOffset.y) != 0.0f)
			{
				snake->SetHeadingDirection(headingOffset);
			}
			snake->Update(timer);

			/*for (const auto& spawn : mSpawns)
			{
				spawn->
			}*/
		}
	}
}
