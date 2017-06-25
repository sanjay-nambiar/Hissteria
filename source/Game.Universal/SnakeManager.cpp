#include "pch.h"
#include "SnakeManager.h"
#include "Snake.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	SnakeManager::SnakeManager(const std::vector<std::shared_ptr<DX::TextRenderer>>& textRenderers, const std::shared_ptr<SpriteManager>& spriteManager,
		const std::shared_ptr<SpawnManager>& spawnManager, const std::shared_ptr<InputComponent>& gameCommands) :
		GameComponent(nullptr),
		mScoreRenderers(textRenderers), mSpriteManager(spriteManager), mSpawnManager(spawnManager), mInputComponent(gameCommands)
	{
		std::uint32_t index = 1;
		for (const auto& config : ProgramHelper::PlayerConfigs)
		{
			auto snake = make_shared<Snake>(index, config.mType, config.mBlocks, config.mDimension, config.mPosition, config.mHeading,
				config.mHeadColor, config.mBodyColor, mSpriteManager);
			snake->mName = config.mName;
			mSnakes.push_back(snake);
			++index;
		}
	}

	void SnakeManager::Update(const DX::StepTimer& timer)
	{
		std::vector<XMFLOAT2> headingOffsets;
		for (auto& snake : mSnakes)
		{
			headingOffsets.push_back({ 0.0f, 0.0f });
			GetPlayerHeading(snake->mId, headingOffsets[snake->mId - 1]);
		}
		
		// Debug keys
		bool IsDebugAddBodyBlockActive = false;
		bool IsDebugShrinkActive = false;
		bool IsDebugExpandActive = false;
		if (ProgramHelper::IsDebugEnabled)
		{
			if (mInputComponent->IsCommandGiven(1, InputComponent::Command::DebugAddBodyBlock))
			{
				IsDebugAddBodyBlockActive = true;
			}

			if (mInputComponent->IsCommandGiven(1, InputComponent::Command::DebugShrink))
			{
				IsDebugShrinkActive = true;
			}

			if (mInputComponent->IsCommandGiven(1, InputComponent::Command::DebugExpand))
			{
				IsDebugExpandActive = true;
			}
		}

		for (auto& snake : mSnakes)
		{
			if ((headingOffsets[snake->mId - 1].x + headingOffsets[snake->mId - 1].y) != 0.0f)
			{
				snake->SetHeadingDirection(headingOffsets[snake->mId - 1]);
			}
			snake->Update(timer);
			
			if (IsDebugAddBodyBlockActive)
			{
				snake->AddBlocks(1, ColorHelper::Yellow());
			}

			if (IsDebugShrinkActive)
			{
				snake->ShrinkSnake(3, ColorHelper::White());
			}

			if (IsDebugExpandActive)
			{
				snake->AddBlocks(1000, ColorHelper::Purple());
			}
		}

		CheckSpawnCollision();
		SnakeToSnakeCollision();

		for (auto& snake : mSnakes)
		{
			std::wstring scoreText = ProgramHelper::ToWideString(snake->mName) + L" : " + std::to_wstring(snake->mScore) + L"\n";
			mScoreRenderers[snake->mId - 1]->SetText(scoreText, 500, 100);
		}
	}

	void SnakeManager::GetPlayerHeading(std::uint32_t playerId, DirectX::XMFLOAT2& headingOffset)
	{
		if (mInputComponent->IsCommandGiven(playerId, InputComponent::Command::MoveUp))
		{
			headingOffset.y = 1.0f;
		}
		else if (mInputComponent->IsCommandGiven(playerId, InputComponent::Command::MoveDown))
		{
			headingOffset.y = -1.0f;
		}
		else if (mInputComponent->IsCommandGiven(playerId, InputComponent::Command::MoveRight))
		{
			headingOffset.x = 1.0f;
		}
		else if (mInputComponent->IsCommandGiven(playerId, InputComponent::Command::MoveLeft))
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
						snake->AddBlocks(1, ColorHelper::Yellow());
						snake->mScore += 5;
						headSprite->SetColorInterpolation(ColorHelper::Yellow(), 0.2f, 0.2f, 3);
						mInputComponent->VibrateController(snake->mId, 0.2f, 0.15f, 0.15f, 0.15f, 0.15f);
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
			for (std::uint32_t collidee = 0; collidee < mSnakes.size(); ++collidee)
			{
				if (mSnakes[collider]->CheckCollisionWithSnake(mSnakes[collidee]))
				{
					snakesToKill.push_back(mSnakes[collider]);
					mInputComponent->VibrateController(mSnakes[collider]->mId, 0.5f, 1.0f, 1.0f);
				}
			}
		}

		if (snakesToKill.size() > 0)
		{
			return;
		}
	}
}
