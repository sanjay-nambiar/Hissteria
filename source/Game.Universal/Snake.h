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

		enum class BlinkStyle
		{
			HeadOnly,
			HeadAndTail,
			TailOnly,
			FullBody
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

		bool AddBlocks(std::uint32_t blocks);
		void ShrinkSnake(std::uint32_t newBlockCount);
		void BlinkSnake(const DirectX::XMFLOAT4& mBlinkColor, BlinkStyle blinkStyle);
		void Hurt();
		void Kill();
		void Revive(const DirectX::XMFLOAT2& position, std::uint32_t blocks);

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
		std::uint32_t mHealth;
		bool mIsInvincible;
		bool mIsAlive;

		std::shared_ptr<SpriteManager> mSpriteManager;

		static const std::uint32_t MaxBodyBlocks;
		static const std::uint32_t DefaultHealth;
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
