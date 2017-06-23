#include "pch.h"
#include "Spawn.h"
#include "SpriteManager.h"

using namespace DirectX;

namespace DirectXGame
{
	const std::unordered_map<Spawn::SpawnType, XMINT2> Spawn::SpriteTypeToIndexMap = {
		{Food, XMINT2(2, 0)}
	};

	Spawn::Spawn(SpawnType type, std::weak_ptr<SpriteManager> spriteManager)
	{
		mType = type;
		mSprite = spriteManager.lock()->CreateSprite(SpriteTypeToIndexMap.at(mType));
	}
}
