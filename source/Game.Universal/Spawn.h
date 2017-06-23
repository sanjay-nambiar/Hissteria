#pragma once

#include <memory>
#include <unordered_map>

namespace DirectXGame
{
	class Sprite;
	class SpriteManager;
	class SpawnManager;

	class Spawn
	{
	public:
		enum SpawnType
		{
			Food
		};

		Spawn(SpawnType type, std::weak_ptr<SpriteManager> spriteManager);
	private:
		SpawnType mType;
		std::weak_ptr<Sprite> mSprite;

		static const std::unordered_map<SpawnType, DirectX::XMINT2> SpriteTypeToIndexMap;

		friend SpawnManager;
	};
}
