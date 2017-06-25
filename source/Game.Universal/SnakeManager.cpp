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
		auto snake = make_shared<Snake>(Snake::SnakeType::ChainLink, 3, dimension, heading, ColorHelper::Blue, ColorHelper::Green, mSpriteManager);
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
		
		// Debug keys
		bool wasPressedA = false;
		bool wasPressedMinus = false;
		bool wasPressedPlus = false;
		if (ProgramHelper::IsDebugEnabled)
		{
			if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::A))
			{
				wasPressedA = true;
			}

			if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::OemMinus))
			{
				wasPressedMinus = true;
			}

			if (mKeyboardComponent->WasKeyPressedThisFrame(Keys::OemPlus))
			{
				wasPressedPlus = true;
			}
		}

		for (auto& snake : mSnakes)
		{
			if ((headingOffset.x + headingOffset.y) != 0.0f)
			{
				snake->SetHeadingDirection(headingOffset);
			}
			snake->Update(timer);
			
			if (wasPressedA)
			{
				snake->AddBlocks(1, ColorHelper::Yellow);
			}

			if (wasPressedMinus)
			{
				snake->ShrinkSnake(3, ColorHelper::White);
			}

			if (wasPressedPlus)
			{
				snake->AddBlocks(1000, ColorHelper::Purple);
			}
		}

		CheckSpawnCollision();
		SnakeToSnakeCollision();
	}

	void SnakeManager::CheckSpawnCollision()
	{
		std::vector<std::shared_ptr<Spawn>> spawnsToUpdate;
		for (auto& snake : mSnakes)
		{
			const auto& headSprite = snake->mBody.front().mSprite.lock();
			const auto& snakePosition = headSprite->Transform().Position();

			for (const auto& spawn : mSpawnManager->Spawns())
			{
				float lengthSq = XMVectorGetX(XMVector2LengthSq(XMLoadFloat2(&snakePosition) - XMLoadFloat2(&spawn->Position())));
				float distanceSq = (snake->ColliderRadius() + spawn->ColliderRadius());
				distanceSq *= distanceSq;

				if (distanceSq >= lengthSq)
				{
					spawnsToUpdate.push_back(spawn);

					switch (spawn->Type())
					{
					case Spawn::SpawnType::Food:
						snake->AddBlocks(1, ColorHelper::Yellow);
						headSprite->SetColorInterpolation(ColorHelper::Yellow, 0.2f, 0.2f, 3);
						break;
					}
				}
			}
		}

		if (spawnsToUpdate.size() > 0)
		{
			mSpawnManager->UpdateSpawnLocations(spawnsToUpdate);
		}
	}

	void SnakeManager::SnakeToSnakeCollision()
	{
		vector<shared_ptr<Snake>> snakesToKill;
		for (std::uint32_t collider = 0; collider < mSnakes.size(); ++collider)
		{
			for (std::uint32_t collidee = collider; collidee < mSnakes.size(); ++collidee)
			{
				if (mSnakes[collider]->CheckCollisionWithSnake(mSnakes[collidee]))
				{
					snakesToKill.push_back(mSnakes[collider]);
				}
			}
		}

		if (snakesToKill.size() > 0)
		{
			return;
		}
	}
}
