#include "pch.h"
#include "SnakeManager.h"
#include "Snake.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	SnakeManager::SnakeManager(const vector<shared_ptr<TextRenderer>>& textRenderers, const shared_ptr<SpriteManager>& spriteManager,
		const shared_ptr<SpawnManager>& spawnManager, const shared_ptr<InputComponent>& gameCommands, const shared_ptr<TimerComponent>& timerComponent) :
		GameComponent(nullptr),
		mWinner(nullptr), mScoreRenderers(textRenderers), mSpriteManager(spriteManager),
		mSpawnManager(spawnManager), mInputComponent(gameCommands), mTimerComponent(timerComponent)
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
			if (!snake->mIsAlive)
			{
				continue;
			}
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
			if (!snake->mIsAlive)
			{
				continue;
			}

			if ((headingOffsets[snake->mId - 1].x + headingOffsets[snake->mId - 1].y) != 0.0f)
			{
				snake->SetHeadingDirection(headingOffsets[snake->mId - 1]);
			}
			snake->Update(timer);
			
			if (IsDebugAddBodyBlockActive)
			{
				if (snake->AddBlocks(1))
				{
					snake->BlinkSnake(ColorHelper::Yellow(), Snake::BlinkStyle::HeadAndTail);
				}
				else
				{
					snake->BlinkSnake(ColorHelper::Yellow(), Snake::BlinkStyle::HeadOnly);
				}
			}

			if (IsDebugShrinkActive)
			{
				snake->ShrinkSnake(3);
				snake->BlinkSnake(ColorHelper::White(), Snake::BlinkStyle::FullBody);
			}

			if (IsDebugExpandActive)
			{
				snake->AddBlocks(1000);
				snake->BlinkSnake(ColorHelper::Gray(), Snake::BlinkStyle::FullBody);
			}
		}

		CheckSpawnCollision();
		SnakeToSnakeCollision();

		for (auto& snake : mSnakes)
		{
			std::wstring scoreText = ProgramHelper::ToWideString(snake->mName) + L" : " + std::to_wstring(snake->mScore) + L"\nLives : " + std::to_wstring(snake->mHealth);
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
			if (!snake->mIsAlive)
			{
				continue;
			}

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
						snake->mScore += 5;
						if (snake->AddBlocks(1))
						{
							snake->BlinkSnake(ColorHelper::Yellow(), Snake::BlinkStyle::HeadAndTail);
						}
						else
						{
							snake->BlinkSnake(ColorHelper::Yellow(), Snake::BlinkStyle::HeadOnly);
						}
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
		for (std::uint32_t collider = 0; collider < mSnakes.size(); ++collider)
		{
			auto& snake = mSnakes[collider];
			if (snake->mIsInvincible || !snake->mIsAlive)
			{
				continue;
			}

			for (std::uint32_t collidee = 0; collidee < mSnakes.size(); ++collidee)
			{
				if (mSnakes[collidee]->mIsAlive && snake->CheckCollisionWithSnake(mSnakes[collidee]))
				{
					mInputComponent->VibrateController(snake->mId, 0.5f, 1.0f, 1.0f);
					snake->Hurt();
					if (snake->mHealth == 0)
					{
						MarkSnakeForKill(snake);
					}
					else
					{
						MakeSnakeInvincible(snake);
					}
				}
			}
		}
	}

	void SnakeManager::MarkSnakeForKill(const shared_ptr<Snake>& snake)
	{
		snake->mSpeed = 0;
		snake->mIsInvincible = true;
		TimerComponent::CallbackSignature callback = bind(&SnakeManager::KillSnake, this, placeholders::_1);
		mTimerComponent->AddTimer(callback, snake.get(), 3.0f, 1);
		snake->BlinkSnake(ColorHelper::Red(), Snake::BlinkStyle::FullBody);
	}

	void SnakeManager::MakeSnakeInvincible(const shared_ptr<Snake>& snake)
	{
		snake->mIsInvincible = true;
		TimerComponent::CallbackSignature callback = bind(&SnakeManager::MakeSnakeVulnerable, this, placeholders::_1);
		mTimerComponent->AddTimer(callback, snake.get(), 2.0f, 1);
		snake->BlinkSnake(ColorHelper::Red(), Snake::BlinkStyle::HeadOnly);
	}

	void SnakeManager::KillSnake(void* data)
	{
		Snake* snake = static_cast<Snake*>(data);
		snake->Kill();
	}

	void SnakeManager::MakeSnakeVulnerable(void* data)
	{
		Snake* snake = static_cast<Snake*>(data);
		snake->mIsInvincible = false;
	}
}
