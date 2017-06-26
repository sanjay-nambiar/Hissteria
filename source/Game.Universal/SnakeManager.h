#pragma once

#include "GameComponent.h"
#include "MatrixHelper.h"
#include <memory>
#include <vector>
#include <unordered_map>

namespace DirectX
{
	class AudioEngine;
	class SoundEffect;
	class SoundEffectInstance;
}

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
		void MarkSnakeForKill(const std::shared_ptr<Snake>& snake);
		void MakeSnakeInvincible(const std::shared_ptr<Snake>& snake);
		void KillSnake(void* snake);
		void MakeSnakeVulnerable(void* snake);

		std::vector<std::shared_ptr<Snake>> mSnakes;
		std::shared_ptr<SpriteManager> mSpriteManager;
		std::shared_ptr<SpawnManager> mSpawnManager;
		std::shared_ptr<Snake> mWinner;

		std::vector<std::shared_ptr<DX::TextRenderer>> mScoreRenderers;

		std::shared_ptr<InputComponent> mInputComponent;
		std::shared_ptr<TimerComponent> mTimerComponent;
		std::unique_ptr<DirectX::AudioEngine> mAudioEngine;

		enum class SoundType
		{
			Music,
			Consume,
			Crash
		};

		std::unordered_map<SoundType, std::unique_ptr<DirectX::SoundEffect>> mSoundEffects;
		std::unique_ptr<DirectX::SoundEffectInstance> mMusicInstance;

		static const std::unordered_map<SoundType, std::wstring> SoundEffectFiles;
	};
}
