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
		void Update(const DX::StepTimer& timer) override;

	private:
		std::vector<std::shared_ptr<Spawn>> mSpawns;

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
