#include "pch.h"
#include "Snake.h"

using namespace DirectX;
using namespace DX;

namespace DirectXGame
{
	const std::uint32_t Snake::MaxBodyBlocks = 20;
	const float Snake::MaxSpeed = 50.0f;
	const float Snake::MaxForce = 5.0f;

	Snake::Snake(std::uint32_t bodyBlocks, XMFLOAT2 blockDimension, XMINT2 facing, const std::shared_ptr<SpriteManager>& spriteManager) :
		mDimension(blockDimension), mHeadingDirection({ -1.0f, 0.0f }), mSpeed(MaxSpeed),
		mSpriteManager(spriteManager), mBlockSeparation(0.0f)
	{
		assert(bodyBlocks > 0 && bodyBlocks < MaxBodyBlocks);
		mBody.reserve(bodyBlocks);

		XMFLOAT2 positionFloat = { 0.0f, 0.0f };
		XMVECTOR blockOffset;
		
		// calculate block offset and offset position to sprite's center
		{
			XMVECTOR blockDimensionVector = XMLoadFloat2(&blockDimension);
			XMFLOAT2 facingFloat = { static_cast<float>(facing.x), static_cast<float>(facing.y) };
			XMVECTOR facingVector = XMLoadFloat2(&facingFloat);
			XMVECTOR centerOffset = { 0.5f, 0.5f };
			centerOffset *= blockDimensionVector;

			blockOffset = facingVector * blockDimensionVector;
			XMStoreFloat2(&mBlockOffset, blockOffset);
			XMStoreFloat2(&positionFloat, centerOffset);
			mBlockSeparation = XMVectorGetX(XMVector2Length(blockDimensionVector));

			BodyBlock block;
			block.mSprite = mSpriteManager->CreateSprite(XMINT2(0, 0));
			auto sprite = block.mSprite.lock();
			sprite->SetColor(ColorHelper::Blue);

			auto transform = Transform2D();
			XMVECTOR position = centerOffset;
			XMVECTOR heading = -XMVector2Normalize(XMLoadFloat2(&mHeadingDirection)) * XMLoadFloat2(&mBlockOffset);

			XMStoreFloat2(&positionFloat, position);
			transform.SetPosition(positionFloat);
			transform.SetScale(XMFLOAT2(3.0f, 3.0f));
			sprite->SetTransform(transform);
			mBody.emplace_back(block);
		}

		for (std::uint32_t i = 1; i < bodyBlocks; ++i)
		{
			AddBlock();
		}
	}

	void Snake::AddBlock()
	{
		BodyBlock block;
		block.mSprite = mSpriteManager->CreateSprite(XMINT2(0, 0));
		auto sprite = block.mSprite.lock();
		sprite->SetColor(ColorHelper::Blue);

		auto transform = Transform2D();
		XMVECTOR position = XMLoadFloat2(&mBody.back().mSprite.lock()->Transform().Position());
		XMVECTOR heading = -XMVector2Normalize(XMLoadFloat2(&mHeadingDirection)) * XMLoadFloat2(&mBlockOffset);
		position += heading;
		XMFLOAT2 positionFloat;
		XMStoreFloat2(&positionFloat, position);
		transform.SetPosition(positionFloat);
		transform.SetScale(XMFLOAT2(3.0f, 3.0f));
		sprite->SetTransform(transform);

		mBody.emplace_back(block);
	}

	void Snake::SetHeadingDirection(DirectX::XMFLOAT2 headingDirection)
	{
		if (headingDirection.x == -mHeadingDirection.x && headingDirection.x != 0)
		{
			return;
		}
		if (headingDirection.y == -mHeadingDirection.y && headingDirection.y != 0)
		{
			return;
		}

		XMVECTOR direction = XMVector2Normalize(XMLoadFloat2(&headingDirection));
		XMStoreFloat2(&mHeadingDirection, direction);
	}

	const DirectX::XMFLOAT2& Snake::HeadingDirection()
	{
		return mHeadingDirection;
	}

	void Snake::Update(const StepTimer& timer)
	{
		XMVECTOR velocity = XMVector2Normalize(XMLoadFloat2(&mHeadingDirection)) * mSpeed;
		XMVECTOR prevPosition;
		XMVECTOR prevBlockNewPosition;
		float deltaPositionLength;

		{
			// Update head
			auto sprite = mBody.front().mSprite.lock();
			auto transform = sprite->Transform();

			XMVECTOR position = XMLoadFloat2(&transform.Position());
			prevPosition = position;
			position = position + (velocity * static_cast<float>(timer.GetElapsedSeconds()));

			XMFLOAT2 positionFloat;
			XMStoreFloat2(&positionFloat, position);
			transform.SetPosition(positionFloat);
			sprite->SetTransform(transform);

			deltaPositionLength = XMVectorGetX(XMVector2Length(position - prevPosition));
			prevBlockNewPosition = position;
		}

		for (std::uint32_t index = 1; index < mBody.size(); ++index)
		{
			auto sprite = mBody[index].mSprite.lock();
			auto transform = sprite->Transform();

			XMVECTOR position = XMLoadFloat2(&transform.Position());
			XMVECTOR direction = XMVector2Normalize(prevPosition - position);
			prevPosition = position;
			position += (direction * deltaPositionLength);

			XMVECTOR delta = XMVector2Normalize(position - prevBlockNewPosition) * mBlockSeparation;
			position = prevBlockNewPosition + delta;
			prevBlockNewPosition = position;

			XMFLOAT2 positionFloat;
			XMStoreFloat2(&positionFloat, position);
			transform.SetPosition(positionFloat);
			sprite->SetTransform(transform);
		}
	}
}
