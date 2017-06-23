#include "pch.h"
#include "Snake.h"

using namespace DirectX;
using namespace DX;

namespace DirectXGame
{
	const std::uint32_t Snake::MaxBodyBlocks = 20;

	Snake::Snake(std::uint32_t bodyBlocks, XMFLOAT2 blockDimension, XMINT2 facing, const std::shared_ptr<SpriteManager>& spriteManager) :
		mDimension(blockDimension), mVelocity({ 0.0f, 0.0f }), mSpriteManager(spriteManager)
	{
		assert(bodyBlocks > 0 && bodyBlocks < MaxBodyBlocks);
		mBody.reserve(bodyBlocks);

		XMFLOAT2 position = { 0.0f, 0.0f };
		XMVECTOR blockOffset;
		
		// calculate block offset and offset position to sprite's center
		{
			XMVECTOR blockDimensionVector = XMLoadFloat2(&blockDimension);
			XMFLOAT2 facingFloat = { static_cast<float>(facing.x), static_cast<float>(facing.y) };
			XMVECTOR facingVector = XMLoadFloat2(&facingFloat);
			XMVECTOR centerOffset = { 0.5f, 0.5f };
			centerOffset *= blockDimensionVector;

			blockOffset = facingVector * blockDimensionVector;
			XMStoreFloat2(&position, centerOffset);
		}

		for (std::uint32_t i = 0; i < bodyBlocks; ++i)
		{
			BodyBlock block;
			block.mSprite = mSpriteManager->CreateSprite(XMINT2(1, 0));
			auto sprite = block.mSprite.lock();
			sprite->SetColor(ColorHelper::Blue);

			auto transform = Transform2D();
			transform.SetPosition(position);
			transform.SetScale(XMFLOAT2(3.0f, 3.0f));
			sprite->SetTransform(transform);
			
			mBody.emplace_back(block);

			XMVECTOR positionVector = XMLoadFloat2(&position);
			positionVector += blockOffset;
			XMStoreFloat2(&position, positionVector);
		}
	}
}
