#include "pch.h"
#include "SnakeManager.h"
#include "Snake.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	SnakeManager::SnakeManager(const std::shared_ptr<SpriteManager>& spriteManager, const std::shared_ptr<SpawnManager>& spawnManager,
		const std::shared_ptr<InputComponent>& gameCommands) :
		GameComponent(nullptr),
		mSpriteManager(spriteManager), mSpawnManager(spawnManager), mGameCommands(gameCommands)
	{
		XMFLOAT2 dimension = { 6.0f, 3.0f };
		XMFLOAT2 heading = { 1.0f, 0.0f };
		auto snake = make_shared<Snake>(Snake::SnakeType::ChainLink, 3, dimension, heading, ColorHelper::Blue, ColorHelper::Green, mSpriteManager);
		mSnakes.push_back(snake);
	}

	void SnakeManager::Update(const DX::StepTimer& timer)
	{
		std::vector<XMFLOAT2> headingOffsets;
		for (uint32_t index = 0; index < mSnakes.size(); ++index)
		{
			headingOffsets.push_back({ 0.0f, 0.0f });
			GetPlayerHeading((index + 1), headingOffsets[index]);
		}
		
		// Debug keys
		bool IsDebugAddBodyBlockActive = false;
		bool IsDebugShrinkActive = false;
		bool IsDebugExpandActive = false;
		if (ProgramHelper::IsDebugEnabled)
		{
			if (mGameCommands->IsCommandGiven(1, InputComponent::Command::DebugAddBodyBlock))
			{
				IsDebugAddBodyBlockActive = true;
			}

			if (mGameCommands->IsCommandGiven(1, InputComponent::Command::DebugShrink))
			{
				IsDebugShrinkActive = true;
			}

			if (mGameCommands->IsCommandGiven(1, InputComponent::Command::DebugExpand))
			{
				IsDebugExpandActive = true;
			}
		}

		for (uint32_t index = 0; index < mSnakes.size(); ++index)
		{
			auto& snake = mSnakes[index];
			if ((headingOffsets[index].x + headingOffsets[index].y) != 0.0f)
			{
				snake->SetHeadingDirection(headingOffsets[index]);
			}
			snake->Update(timer);
			
			if (IsDebugAddBodyBlockActive)
			{
				snake->AddBlocks(1, ColorHelper::Yellow);
			}

			if (IsDebugShrinkActive)
			{
				snake->ShrinkSnake(3, ColorHelper::White);
			}

			if (IsDebugExpandActive)
			{
				snake->AddBlocks(1000, ColorHelper::Purple);
			}
		}

		CheckSpawnCollision();
		SnakeToSnakeCollision();
	}

	void SnakeManager::GetPlayerHeading(std::uint32_t playerId, DirectX::XMFLOAT2& headingOffset)
	{
		if (mGameCommands->IsCommandGiven(playerId, InputComponent::Command::MoveUp))
		{
			headingOffset.y = 1.0f;
		}
		else if (mGameCommands->IsCommandGiven(playerId, InputComponent::Command::MoveDown))
		{
			headingOffset.y = -1.0f;
		}
		else if (mGameCommands->IsCommandGiven(playerId, InputComponent::Command::MoveRight))
		{
			headingOffset.x = 1.0f;
		}
		else if (mGameCommands->IsCommandGiven(playerId, InputComponent::Command::MoveLeft))
		{
			headingOffset.x = -1.0f;
		}
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
					case Spawn::SpawnType::Grow:
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
