#pragma once

#include <memory>

namespace DirectXGame
{
	class Sprite;
	class SpriteManager;
	class SnakeManager;

	class Snake
	{
	public:
		enum class SnakeType
		{
			Circular,
			ChainLink
		};

		Snake(SnakeType type, std::uint32_t bodyBlocks, DirectX::XMFLOAT2 blockDimension, DirectX::XMINT2 facing, const std::shared_ptr<SpriteManager>& spriteManager);

		void SetHeadingDirection(DirectX::XMFLOAT2 headingDirection);
		void AddBlock();

		const DirectX::XMFLOAT2& HeadingDirection();
		SnakeType Type() const;

		void Update(const DX::StepTimer& timer);
	private:
		struct BodyBlock
		{
			std::weak_ptr<Sprite> mSprite;
		};

		struct SnakeTypeConfig
		{
			const DirectX::XMINT2 mSpriteIndex;
			const float mColliderRadius;
		};

		SnakeType mType;
		std::vector<BodyBlock> mBody;
		DirectX::XMFLOAT2 mDimension;
		DirectX::XMFLOAT2 mHeadingDirection;
		float mSpeed;
		const float mColliderRadius;

		std::shared_ptr<SpriteManager> mSpriteManager;
		float mBlockSeparation;
		DirectX::XMFLOAT2 mBlockOffset;

		static const std::uint32_t MaxBodyBlocks;
		static const float MaxSpeed;
		static const float MaxForce;
		static const float ColliderRadius;

		static const std::unordered_map<SnakeType, SnakeTypeConfig> SnakeTypeConfigMapping;

		friend SnakeManager;
	};
}
