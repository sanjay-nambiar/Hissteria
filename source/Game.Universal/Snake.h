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
		Snake(std::uint32_t bodyBlocks, DirectX::XMFLOAT2 blockDimension, DirectX::XMINT2 facing, const std::shared_ptr<SpriteManager>& spriteManager);

		void SetHeadingDirection(DirectX::XMFLOAT2 headingDirection);

		void AddBlock();

		const DirectX::XMFLOAT2& HeadingDirection();

		void Update(const DX::StepTimer& timer);
	private:
		struct BodyBlock
		{
			std::weak_ptr<Sprite> mSprite;
		};

		std::vector<BodyBlock> mBody;
		DirectX::XMFLOAT2 mDimension;
		DirectX::XMFLOAT2 mHeadingDirection;
		float mSpeed;

		std::shared_ptr<SpriteManager> mSpriteManager;
		float mBlockSeparation;
		DirectX::XMFLOAT2 mBlockOffset;

		static const std::uint32_t MaxBodyBlocks;
		static const float MaxSpeed;
		static const float MaxForce;

		friend SnakeManager;
	};
}
