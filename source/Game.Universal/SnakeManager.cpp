#include "pch.h"
#include "SnakeManager.h"
#include "Snake.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	const std::unordered_map<SnakeManager::SoundType, std::wstring> SnakeManager::SoundEffectFiles = {
		{ SoundType::Music, L"Content\\Audio\\Arcade-Fantasy.wav" },
		{ SoundType::Consume, L"Content\\Audio\\Consume.wav" },
		{ SoundType::Crash, L"Content\\Audio\\Crash.wav" }
	};

	const uint32_t SnakeManager::MaxCountForCountDown = 3;
	const uint32_t SnakeManager::TargetScoreToAchieve = 200;

	SnakeManager::SnakeManager(const vector<shared_ptr<TextRenderer>>& textRenderers, const shared_ptr<SpriteManager>& spriteManager,
		const shared_ptr<SpawnManager>& spawnManager, const shared_ptr<InputComponent>& gameCommands, const shared_ptr<TimerComponent>& timerComponent) :
		GameComponent(nullptr),
		mWinner(nullptr), mTextRenderers(textRenderers), mSpriteManager(spriteManager),
		mSpawnManager(spawnManager), mInputComponent(gameCommands), mTimerComponent(timerComponent),
		mAudioEngine(make_unique<AudioEngine>()), mRoundState(RoundState::RoundBegin), mCount(0)
	{
		mSnakes.reserve(ProgramHelper::PlayerConfigs.size());
		mPlayerPressedStart.reserve(ProgramHelper::PlayerConfigs.size());
		std::uint32_t index = 1;
		for (const auto& config : ProgramHelper::PlayerConfigs)
		{
			auto snake = make_shared<Snake>(index, config.mType, config.mBlocks, config.mDimension, config.mPosition, config.mHeading,
				config.mHeadColor, config.mBodyColor, mSpriteManager);
			snake->mName = config.mName;
			mSnakes.push_back(snake);
			mPlayerPressedStart.push_back(false);
			++index;
		}

		for (auto& textRenderer : mTextRenderers)
		{
			textRenderer->SetVisible(false);
		}
		mTextRenderers[mTextRenderers.size() - 2]->SetText(L"Target Score: " + std::to_wstring(TargetScoreToAchieve), 350, 80);
		mTextRenderers.back()->SetVisible(true);
		mTextRenderers.back()->SetText(L"Press Start", 350, 80);
		
		for (const auto& entry : SoundEffectFiles)
		{
			mSoundEffects.insert({ entry.first, std::make_unique<SoundEffect>(mAudioEngine.get(), entry.second.c_str()) });
		}
		mMusicInstance = mSoundEffects[SoundType::Music]->CreateInstance();
		mMusicInstance->SetVolume(0.5f);
		mMusicInstance->Play(true);
	}

	void SnakeManager::Initialize()
	{
		for (uint32_t index = 0; index < ProgramHelper::PlayerConfigs.size(); ++index)
		{
			const auto& config = ProgramHelper::PlayerConfigs[index];
			auto& snake = mSnakes[index];
			snake->mType = config.mType;
			snake->mHeadingDirection = config.mHeading;
			snake->mHeadColor = config.mHeadColor;
			snake->mBodyColor = config.mBodyColor;
			snake->Revive(config.mPosition, config.mBlocks);
			mPlayerPressedStart[index] = false;
		}
		mSpawnManager->Initialize();
		mTextRenderers.back()->SetText(L"Press Start", 350, 80);
		mRoundState = RoundState::RoundBegin;
		mWinner = nullptr;
		mCount = 0;
	}

	void SnakeManager::Update(const DX::StepTimer& timer)
	{
		switch (mRoundState)
		{
		case RoundState::RoundBegin:
			RoundBeginUpdate(timer);
			break;

		case RoundState::InGame:
			InGameUpdate(timer);
			break;

		case RoundState::RoundEnd:
			RoundEndUpdate(timer);
			break;
		}

		// Score update
		for (auto& snake : mSnakes)
		{
			std::wstring scoreText = ProgramHelper::ToWideString(snake->mName) + L" : " + std::to_wstring(snake->mScore) + L"\nLives : " + std::to_wstring(snake->mHealth);
			mTextRenderers[snake->mId - 1]->SetText(scoreText, 500, 100);
		}
		mAudioEngine->Update();
	}

	void SnakeManager::RoundBeginUpdate(const DX::StepTimer&)
	{
		bool allPlayersReady = true;
		for (uint32_t index = 0; index < mSnakes.size(); ++index)
		{
			auto& snake = mSnakes[index];
			bool playerPressedStart = mInputComponent->IsCommandGiven(snake->mId, InputComponent::Command::StartRound);
			if (!mPlayerPressedStart[index] && playerPressedStart)
			{
				mSoundEffects[SoundType::Consume]->Play();
				mSnakes[index]->BlinkSnake(ColorHelper::White(), Snake::BlinkStyle::FullBody);
			}
			mPlayerPressedStart[index] = mPlayerPressedStart[index] | playerPressedStart;
			allPlayersReady &= mPlayerPressedStart[index];
		}

		if (allPlayersReady)
		{
			mCount = 0;
			TimerComponent::CallbackSignature callback = bind(&SnakeManager::StartCountdown, this, placeholders::_1);
			mTimerComponent->AddTimer(callback, nullptr, 1.2f, MaxCountForCountDown);

			for (auto& textRenderer : mTextRenderers)
			{
				textRenderer->SetVisible(true);
			}
		}
	}

	void SnakeManager::InGameUpdate(const StepTimer& timer)
	{
		if (mWinner != nullptr)
		{
			return;
		}

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
	}

	void SnakeManager::RoundEndUpdate(const DX::StepTimer&)
	{
		if (mInputComponent->IsCommandGiven(0, InputComponent::Command::RestartRound))
		{
			Initialize();
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
						snake->mScore += spawn->Points();
						if (snake->AddBlocks(1))
						{
							snake->BlinkSnake(ColorHelper::Yellow(), Snake::BlinkStyle::HeadAndTail);
						}
						else
						{
							snake->BlinkSnake(ColorHelper::Yellow(), Snake::BlinkStyle::HeadOnly);
						}
						mInputComponent->VibrateController(snake->mId, 0.2f, 0.15f, 0.15f, 0.15f, 0.15f);
						mSoundEffects[SoundType::Consume]->Play();
						
						if (snake->mScore >= TargetScoreToAchieve)
						{
							SetWinner(snake);
							break;
						}
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
		
		mSoundEffects[SoundType::Crash]->Play();
	}

	void SnakeManager::MakeSnakeInvincible(const shared_ptr<Snake>& snake)
	{
		snake->mIsInvincible = true;
		TimerComponent::CallbackSignature callback = bind(&SnakeManager::MakeSnakeVulnerable, this, placeholders::_1);
		mTimerComponent->AddTimer(callback, snake.get(), 1.2f, 1);
		snake->BlinkSnake(ColorHelper::Red(), Snake::BlinkStyle::HeadOnly);
		mSoundEffects[SoundType::Crash]->Play();
	}

	void SnakeManager::KillSnake(void* data)
	{
		{
			Snake* snake = static_cast<Snake*>(data);
			snake->Kill();
		}

		std::uint32_t aliveSnakes = 0;
		std::shared_ptr<Snake> winner;
		for (auto& snake : mSnakes)
		{
			if (snake->mIsAlive)
			{
				++aliveSnakes;
				winner = snake;
			}
		}

		if (aliveSnakes == 1)
		{
			SetWinner(winner);
		}
	}

	void SnakeManager::SetWinner(std::shared_ptr<DirectXGame::Snake> &winner)
	{
		mWinner = winner;
		for (auto& textRenderer : mTextRenderers)
		{
			textRenderer->SetVisible(false);
		}
		mTextRenderers.back()->SetText(L" Winner : " + ProgramHelper::ToWideString(mWinner->mName), 500, 50);
		mTextRenderers.back()->SetVisible(true);
		mRoundState = RoundState::RoundEnd;
		mSpawnManager->SetSpawnEnabled(false);
	}

	void SnakeManager::MakeSnakeVulnerable(void* data)
	{
		Snake* snake = static_cast<Snake*>(data);
		snake->mIsInvincible = false;
	}

	void SnakeManager::StartCountdown(void*)
	{
		mTextRenderers.back()->SetText(std::to_wstring(MaxCountForCountDown - mCount), 50, 50);
		++mCount;

		if (mCount == MaxCountForCountDown)
		{
			mRoundState = RoundState::InGame;
			
			for (auto& snake : mSnakes)
			{
				snake->BlinkSnake(ColorHelper::White(), Snake::BlinkStyle::FullBody);
			}
			mSoundEffects[SoundType::Consume]->Play();
			mSpawnManager->SetSpawnEnabled(true);
			mTextRenderers.back()->SetVisible(false);
		}
	}
}
