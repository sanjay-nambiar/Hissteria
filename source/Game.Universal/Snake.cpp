#include "pch.h"
#include "Snake.h"

using namespace DirectX;
using namespace DX;

namespace DirectXGame
{
	const std::uint32_t Snake::MaxBodyBlocks = 20;
	const XMFLOAT2 Snake::MaxVelocity = { 30.0f, 25.0f };
	const XMFLOAT2 Snake::MaxForce = { 100.0f, 100.0f };

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
		XMVECTOR maxVelocity = XMLoadFloat2(&MaxVelocity);
		XMVECTOR maxForce = XMLoadFloat2(&MaxForce);
		XMVECTOR velocity = XMLoadFloat2(&mVelocity);
		XMVECTOR prevPosition;
		float deltaPositionLength;

		{
			// Update head
			auto sprite = mBody.front().mSprite.lock();
			auto transform = sprite->Transform();

			XMVECTOR position = XMLoadFloat2(&transform.Position());
			prevPosition = position;
			XMVECTOR target = position + (XMLoadFloat2(&mHeadingDirection) * 10);
			XMVECTOR desiredVelocity = XMVector2Normalize(target - position) * maxVelocity;
			XMVECTOR steering = desiredVelocity - velocity;
			XMFLOAT2 steeringFloat;
			XMStoreFloat2(&steeringFloat, steering);

			steeringFloat.x = (steeringFloat.x > MaxForce.x) ? MaxForce.x : steeringFloat.x;
			steeringFloat.y = (steeringFloat.y > MaxForce.y) ? MaxForce.y : steeringFloat.y;
			steering = XMLoadFloat2(&steeringFloat);
			// steering = steering / 1.0f; // 1.0f is mass here

			XMFLOAT2 velocityFloat;
			XMStoreFloat2(&velocityFloat, (velocity + steering));
			velocityFloat.x = velocityFloat.x > MaxVelocity.x ? MaxVelocity.x : velocityFloat.x;
			velocityFloat.y = velocityFloat.y > MaxVelocity.y ? MaxVelocity.y : velocityFloat.y;
			velocity = XMLoadFloat2(&velocityFloat);
			position = position + velocity * static_cast<float>(timer.GetElapsedSeconds());

			XMFLOAT2 positionFloat;
			XMStoreFloat2(&positionFloat, position);
			transform.SetPosition(positionFloat);
			sprite->SetTransform(transform);

			deltaPositionLength = XMVectorGetX(XMVector2Length(position - prevPosition));
		}

		for (std::uint32_t index = 1; index < mBody.size(); ++index)
		{
			auto sprite = mBody[index].mSprite.lock();
			auto transform = sprite->Transform();

			XMVECTOR position = XMLoadFloat2(&transform.Position());
			XMVECTOR direction = XMVector2Normalize(prevPosition - position);
			prevPosition = position;
			position += (direction * deltaPositionLength);

			XMFLOAT2 positionFloat;
			XMStoreFloat2(&positionFloat, position);
			transform.SetPosition(positionFloat);
			sprite->SetTransform(transform);
		}
	}
}
