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
			ChainLink,
			Star
		};

		Snake(SnakeType type, std::uint32_t bodyBlocks, DirectX::XMFLOAT2 blockDimension, DirectX::XMFLOAT2 heading, const std::shared_ptr<SpriteManager>& spriteManager);

		void SetHeadingDirection(DirectX::XMFLOAT2 headingDirection);

		const DirectX::XMFLOAT2& HeadingDirection();
		SnakeType Type() const;
		float ColliderRadius() const;

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

		void AddBlock();
		void ShrinkSnake(std::uint32_t newBlockCount);

		SnakeType mType;
		std::vector<BodyBlock> mBody;
		DirectX::XMFLOAT2 mDimension;
		DirectX::XMFLOAT2 mHeadingDirection;
		float mSpeed;
		const float mColliderRadius;
		float mBlockSeparation;

		std::shared_ptr<SpriteManager> mSpriteManager;

		static const std::uint32_t MaxBodyBlocks;
		static const float MaxSpeed;
		static const float MaxForce;
		static const DirectX::XMFLOAT2 BlockScale;
		static const DirectX::XMFLOAT2 ZeroAngleVector;
		static const std::unordered_map<SnakeType, SnakeTypeConfig> SnakeTypeConfigMapping;

		friend SnakeManager;
	};
}
