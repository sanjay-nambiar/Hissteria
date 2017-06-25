#include "pch.h"
#include "SpawnManager.h"
#include "Spawn.h"
#include "SpriteManager.h"

using namespace std;
using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	const float SpawnManager::SpawnLifeTimeSeconds = 50.0f;
	const XMFLOAT2 SpawnManager::SpawnScale = XMFLOAT2(3.0f, 3.0f);

	SpawnManager::SpawnManager(std::uint32_t maxSpawns, std::shared_ptr<SpriteManager> spriteManager) :
		GameComponent(nullptr),
		mRandomRealDistribution(0, 100.0f), mRandomIntDistribution(0, 100),
		mRandomGenerator(static_cast<uint32_t>(chrono::system_clock::now().time_since_epoch().count())),
		mMaxSpawns(maxSpawns), mSpriteManager(spriteManager), mSecondsSinceLastSpawn(0.0f)
	{
		mSpawns.reserve(mMaxSpawns);
		for (std::uint32_t index = 0; index < maxSpawns; ++index)
		{
			auto spawn = make_shared<Spawn>(Spawn::SpawnType::Food, spriteManager);
			auto sprite = spawn->mSprite.lock();
			auto transform = Transform2D(Vector2Helper::Zero, 0.0f, SpawnScale);
			sprite->SetTransform(transform);
			mSpawns.push_back(spawn);
		}
	}

	void SpawnManager::CreateDeviceDependentResources()
	{
		mSecondsSinceLastSpawn = 0.0f;
	}

	void SpawnManager::ReleaseDeviceDependentResources()
	{
	}

	void SpawnManager::Update(const StepTimer& timer)
	{
		mSecondsSinceLastSpawn += static_cast<float>(timer.GetElapsedSeconds());
		bool updatePosition = (mSecondsSinceLastSpawn >= SpawnLifeTimeSeconds);

		for (auto& spawn : mSpawns)
		{
			shared_ptr<Sprite> sprite = spawn->mSprite.lock();
			auto transform = sprite->Transform();
			transform.SetRotation(transform.Rotation() + 0.05f);
			sprite->SetTransform(transform);
			
			if (updatePosition)
			{
				MoveSpawn(spawn);
			}
		}

		if (updatePosition)
		{
			mSecondsSinceLastSpawn = 0;
		}
	}

	void SpawnManager::MoveSpawn(const shared_ptr<Spawn>& spawn)
	{
		shared_ptr<Sprite> sprite = spawn->mSprite.lock();
		auto transform = sprite->Transform();
		XMFLOAT2 position = ProgramHelper::RandomLocationInsideBorder({spawn->ColliderRadius(), spawn->ColliderRadius()});
		transform.SetPosition(position);
		sprite->SetTransform(transform);
		sprite->SetColor(ColorHelper::RandomColor());
	}

	void SpawnManager::UpdateSpawnLocations(const std::vector<std::shared_ptr<Spawn>>& spawns)
	{
		for (const auto& spawn : spawns)
		{
			MoveSpawn(spawn);
			mSecondsSinceLastSpawn = 0;
		}
	}

	const std::vector<std::shared_ptr<Spawn>>& SpawnManager::Spawns()
	{
		return mSpawns;
	}
}

