#include "pch.h"
#include "Spawn.h"
#include "SpriteManager.h"

using namespace DirectX;

namespace DirectXGame
{
	const std::unordered_map<Spawn::SpawnType, Spawn::SpawnTypeConfig> Spawn::SpawnTypeConfigMapping = {
		{SpawnType::Grow, { XMINT2(2, 0), 3.0f , 5}}
	};

	Spawn::Spawn(SpawnType type, std::weak_ptr<SpriteManager> spriteManager)
	{
		mType = type;
		const auto& foodConfig = SpawnTypeConfigMapping.at(mType);
		mColliderRadius = foodConfig.mColliderRadius;
		mSprite = spriteManager.lock()->CreateSprite(foodConfig.mSpriteIndex);
	}

	Spawn::SpawnType Spawn::Type() const
	{
		return mType;
	}

	uint32_t Spawn::Points() const
	{
		return SpawnTypeConfigMapping.at(mType).mPoints;
	}

	float Spawn::ColliderRadius() const
	{
		return mColliderRadius;
	}

	const XMFLOAT2& Spawn::Position() const
	{
		return mSprite.lock()->Transform().Position();
	}
}
