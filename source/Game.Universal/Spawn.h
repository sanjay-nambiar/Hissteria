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
		enum class SpawnType
		{
			Food
		};

		Spawn(SpawnType type, std::weak_ptr<SpriteManager> spriteManager);

		SpawnType Type() const;
		float ColliderRadius() const;
		const DirectX::XMFLOAT2& Position() const;
	private:
		struct SpawnTypeConfig
		{
			const DirectX::XMINT2 mSpriteIndex;
			const float mColliderRadius;
		};

		SpawnType mType;
		std::weak_ptr<Sprite> mSprite;
		float mColliderRadius;

		static const std::unordered_map<SpawnType, SpawnTypeConfig> SpawnTypeConfigMapping;

		friend SpawnManager;
	};
}
