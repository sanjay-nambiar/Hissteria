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

	private:
		struct BodyBlock
		{
			std::weak_ptr<Sprite> mSprite;
		};

		std::vector<BodyBlock> mBody;
		DirectX::XMFLOAT2 mDimension;
		DirectX::XMFLOAT2 mVelocity;

		std::shared_ptr<SpriteManager> mSpriteManager;

		static const std::uint32_t MaxBodyBlocks;

		friend SnakeManager;
	};
}
