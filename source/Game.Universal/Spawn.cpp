#include "pch.h"
#include "Spawn.h"
#include "SpriteManager.h"

using namespace DirectX;

namespace DirectXGame
{
	const std::unordered_map<Spawn::SpawnType, Spawn::SpawnTypeConfig> Spawn::SpawnTypeConfigMapping = {
		{SpawnType::Food, { XMINT2(2, 0), 40.0f }}
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
}
