#pragma once

#include "GameComponent.h"
#include "MatrixHelper.h"
#include <chrono>
#include <memory>
#include <random>
#include <vector>

namespace DirectXGame
{
	class SpriteManager;

	class SpawnManager final : public DX::GameComponent
	{
	public:
		SpawnManager(std::uint32_t maxSpawns, std::shared_ptr<SpriteManager> spriteManager);

		void CreateDeviceDependentResources() override;
		void ReleaseDeviceDependentResources() override;
		
		void Initialize();
		void Update(const DX::StepTimer& timer) override;

		void SetSpawnEnabled(bool isEnabled);
		void UpdateSpawnLocations(const std::vector<std::shared_ptr<Spawn>>& spawn);
		const std::vector<std::shared_ptr<Spawn>>& Spawns();

	private:
		void MoveSpawn(const std::shared_ptr<Spawn>& spawn);

		std::vector<std::shared_ptr<Spawn>> mSpawns;
		bool mShouldSpawn;

		std::default_random_engine mRandomGenerator;
		std::uniform_real_distribution<float> mRandomRealDistribution;
		std::uniform_int_distribution<std::uint32_t> mRandomIntDistribution;
		float mSecondsSinceLastSpawn;
		std::uint32_t mMaxSpawns;
		std::shared_ptr<SpriteManager> mSpriteManager;

		static const float SpawnLifeTimeSeconds;
		static const DirectX::XMFLOAT2 SpawnScale;
	};
}
