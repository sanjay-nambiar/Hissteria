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

		Snake(std::uint32_t id, SnakeType type, std::uint32_t bodyBlocks, const DirectX::XMFLOAT2& blockDimension, const DirectX::XMFLOAT2& position,
			const DirectX::XMFLOAT2& heading, const DirectX::XMFLOAT4& headColor, const DirectX::XMFLOAT4& bodyColor, const std::shared_ptr<SpriteManager>& spriteManager);

		void SetHeadingDirection(DirectX::XMFLOAT2 headingDirection);

		const DirectX::XMFLOAT2& HeadingDirection();
		SnakeType Type() const;
		float ColliderRadius() const;
		bool CheckCollisionWithSnake(const std::shared_ptr<Snake>& snake);

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

		void AddBlocks(std::uint32_t blocks);
		void AddBlocks(std::uint32_t blocks, const DirectX::XMFLOAT4& mBlinkColor);
		void ShrinkSnake(std::uint32_t newBlockCount);
		void ShrinkSnake(std::uint32_t newBlockCount, const DirectX::XMFLOAT4& mBlinkColor);

		std::uint32_t mId;
		SnakeType mType;
		std::vector<BodyBlock> mBody;
		DirectX::XMFLOAT2 mDimension;
		DirectX::XMFLOAT2 mHeadingDirection;
		float mSpeed;
		const float mColliderRadius;
		float mBlockSeparation;
		DirectX::XMFLOAT4 mHeadColor;
		DirectX::XMFLOAT4 mBodyColor;

		std::string mName;
		std::uint32_t mScore;

		std::shared_ptr<SpriteManager> mSpriteManager;

		static const std::uint32_t MaxBodyBlocks;
		static const float MaxSpeed;
		static const float MaxForce;
		static const float BlinkForwardTime;
		static const float BlinkBackwardTime;
		static const std::uint32_t BlinkCount;
		static const DirectX::XMFLOAT2 BlockScale;
		static const DirectX::XMFLOAT2 ZeroAngleVector;
		static const std::unordered_map<SnakeType, SnakeTypeConfig> SnakeTypeConfigMapping;

		friend SnakeManager;
	};
}
