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
		auto snake = make_shared<Snake>(Snake::SnakeType::ChainLink, 3, dimension, heading, mSpriteManager);
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
		
		bool wasAPressed = false;
		if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::A))
		{
			wasAPressed = true;
		}

		//const auto& mSpawns = mSpawnManager->Spawns();
		for (auto& snake : mSnakes)
		{
			if ((headingOffset.x + headingOffset.y) != 0.0f)
			{
				snake->SetHeadingDirection(headingOffset);
			}
			snake->Update(timer);
			if (wasAPressed)
			{
				snake->AddBlock();
			}
		}

		CheckSpawnCollision();
	}

	void SnakeManager::CheckSpawnCollision()
	{
		std::vector<std::shared_ptr<Spawn>> spawnsToKill;
		for (auto& snake : mSnakes)
		{
			const auto& head = snake->mBody.front();
			const auto& snakePosition = head.mSprite.lock()->Transform().Position();

			for (const auto& spawn : mSpawnManager->Spawns())
			{
				float lengthSq = XMVectorGetX(XMVector2LengthSq(XMLoadFloat2(&snakePosition) - XMLoadFloat2(&spawn->Position())));
				float distanceSq = (snake->ColliderRadius() + spawn->ColliderRadius());
				distanceSq *= distanceSq;

				if (distanceSq >= lengthSq)
				{
					spawnsToKill.push_back(spawn);

					switch (spawn->Type())
					{
					case Spawn::SpawnType::Food:
						snake->AddBlock();
						break;
					}
				}
			}
		}

		if (spawnsToKill.size() > 0)
		{
			mSpawnManager->KillSpawns(spawnsToKill);
		}
	}
}
