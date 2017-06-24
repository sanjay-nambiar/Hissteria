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
		mDimension(blockDimension), mVelocity({ 0.0f, 0.0f }), mSpriteManager(spriteManager), mBlockSeparation(0.0f)
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
			mBlockSeparation = XMVectorGetX(XMVector2Length(blockDimensionVector));
		}

		for (std::uint32_t i = 0; i < bodyBlocks; ++i)
		{
			BodyBlock block;
			block.mSprite = mSpriteManager->CreateSprite(XMINT2(0, 0));
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

	void Snake::SetHeadingDirection(DirectX::XMFLOAT2 headingDirection)
	{
		XMVECTOR direction = XMVector2Normalize(XMLoadFloat2(&headingDirection));
		XMStoreFloat2(&mHeadingDirection, direction);
	}

	const DirectX::XMFLOAT2& Snake::HeadingDirection()
	{
		return mHeadingDirection;
	}

	void Snake::Update(const StepTimer& timer)
	{
		XMVECTOR velocity = XMLoadFloat2(&mVelocity);
		XMVECTOR prevPosition;
		XMVECTOR prevBlockNewPosition;
		float deltaPositionLength;

		{
			// Update head
			auto sprite = mBody.front().mSprite.lock();
			auto transform = sprite->Transform();

			XMVECTOR position = XMLoadFloat2(&transform.Position());
			prevPosition = position;
			XMVECTOR target = position + (XMLoadFloat2(&mHeadingDirection) * 1000);
			XMVECTOR desiredVelocity = XMVector2Normalize(target - position) * MaxSpeed;
			XMVECTOR steering = desiredVelocity - velocity;
			
			float steeringLengthSq = XMVectorGetX(XMVector2LengthSq(steering));
			if (steeringLengthSq > MaxForce)
			{
				steering = MaxForce * XMVector2Normalize(steering);
			}

			velocity += steering;
			float velocityLengthSq = XMVectorGetX(XMVector2LengthSq(velocity));
			if (velocityLengthSq > MaxSpeed)
			{
				velocity = MaxSpeed * XMVector2Normalize(velocity);
			}
			position = position + velocity * static_cast<float>(timer.GetElapsedSeconds());

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
