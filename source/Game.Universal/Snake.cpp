#include "pch.h"
#include "Snake.h"

using namespace DirectX;
using namespace DX;

namespace DirectXGame
{
	const std::uint32_t Snake::MaxBodyBlocks = 10;
	const float Snake::MaxSpeed = 50.0f;
	const float Snake::MaxForce = 5.0f;

	const XMFLOAT2 Snake::BlockScale = { 3.0f, 3.0f };
	const XMFLOAT2 Snake::ZeroAngleVector = { 1.0f, 0.0f };

	const std::unordered_map<Snake::SnakeType, Snake::SnakeTypeConfig> Snake::SnakeTypeConfigMapping = {
		{ SnakeType::Circular, { XMINT2(0, 0), 3.0f } },
		{ SnakeType::ChainLink, { XMINT2(1, 0), 3.0f } }
	};


	Snake::Snake(SnakeType type, std::uint32_t bodyBlocks, XMFLOAT2 blockDimension, XMFLOAT2 heading, const std::shared_ptr<SpriteManager>& spriteManager) :
		mDimension(blockDimension), mHeadingDirection(heading),
		mSpeed(MaxSpeed), mColliderRadius(SnakeTypeConfigMapping.at(type).mColliderRadius), mType(type),
		mSpriteManager(spriteManager), mBlockSeparation(0.0f)
	{
		assert(bodyBlocks > 0 && bodyBlocks < MaxBodyBlocks);
		mBody.reserve(bodyBlocks);

		XMFLOAT2 positionFloat = { 0.0f, 0.0f };
		XMVECTOR centerOffset = { 0.5f, 0.5f };

		// calculate block offset and offset position to sprite's center
		XMVECTOR blockDimensionVector = XMLoadFloat2(&blockDimension);
		XMVECTOR headingVector = XMVector2Normalize(XMLoadFloat2(&mHeadingDirection));
		XMStoreFloat2(&mHeadingDirection, headingVector);
		centerOffset *= blockDimensionVector;

		XMVECTOR blockOffset = -(headingVector * blockDimensionVector);
		XMStoreFloat2(&positionFloat, XMLoadFloat2(&positionFloat) + centerOffset);
		mBlockSeparation = XMVectorGetX(XMVector2Length(blockDimensionVector));

		BodyBlock block;
		block.mSprite = mSpriteManager->CreateSprite(SnakeTypeConfigMapping.at(type).mSpriteIndex);
		auto sprite = block.mSprite.lock();
		sprite->SetColor(ColorHelper::Blue);

		auto transform = Transform2D();
		XMVECTOR position = centerOffset;

		float rotation = XMVectorGetX(XMVector2AngleBetweenNormals(XMLoadFloat2(&ZeroAngleVector), XMLoadFloat2(&mHeadingDirection)));
		if (mHeadingDirection.y < 0)
		{
			rotation = -rotation;
		}
		transform.SetRotation(rotation);

		XMStoreFloat2(&positionFloat, position);
		transform.SetPosition(positionFloat);
		transform.SetScale(BlockScale);
		sprite->SetTransform(transform);
		mBody.emplace_back(block);

		for (std::uint32_t i = 1; i < bodyBlocks; ++i)
		{
			AddBlock();
		}
	}

	void Snake::AddBlock()
	{
		if (mBody.size() >= MaxBodyBlocks)
		{
			return;
		}

		BodyBlock block;
		block.mSprite = mSpriteManager->CreateSprite(SnakeTypeConfigMapping.at(mType).mSpriteIndex);
		auto sprite = block.mSprite.lock();
		sprite->SetColor(ColorHelper::Green);

		auto transform = Transform2D();
		const auto& leadingSprite = mBody.back().mSprite.lock();
		XMVECTOR position = XMLoadFloat2(&leadingSprite->Transform().Position());

		float rotation = leadingSprite->Transform().Rotation();
		transform.SetRotation(rotation);
		XMMATRIX rotationTransform = XMMatrixAffineTransformation2D(XMLoadFloat2(&Vector2Helper::One), XMLoadFloat2(&Vector2Helper::Zero),
			rotation, XMLoadFloat2(&Vector2Helper::Zero));
		XMVECTOR headingVector = XMVector2Transform(XMLoadFloat2(&ZeroAngleVector), rotationTransform);
		XMVECTOR blockOffset = -(headingVector * XMLoadFloat2(&mDimension));
		position += blockOffset;

		XMFLOAT2 positionFloat;
		XMStoreFloat2(&positionFloat, position);
		transform.SetPosition(positionFloat);
		transform.SetScale(BlockScale);
		sprite->SetTransform(transform);

		mBody.emplace_back(block);
	}

	void Snake::ShrinkSnake(std::uint32_t newBlockCount)
	{
		if (newBlockCount > 0 && newBlockCount <= mBody.size())
		{
			for (auto index = mBody.size() - 1; index >= newBlockCount; --index)
			{
				mSpriteManager->RemoveSprite(mBody[index].mSprite);
			}
			mBody.erase(mBody.begin() + newBlockCount, mBody.end());
		}
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

	Snake::SnakeType Snake::Type() const
	{
		return mType;
	}

	float Snake::ColliderRadius() const
	{
		return mColliderRadius;
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

			// update position
			XMFLOAT2 positionFloat;
			XMStoreFloat2(&positionFloat, position);
			transform.SetPosition(positionFloat);
			
			// update rotation
			float rotation = XMVectorGetX(XMVector2AngleBetweenNormals(XMLoadFloat2(&ZeroAngleVector), velocity));
			if (mHeadingDirection.y < 0)
			{
				rotation = -rotation;
			}
			transform.SetRotation(rotation);

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

			// update position
			XMFLOAT2 positionFloat;
			XMStoreFloat2(&positionFloat, position);
			transform.SetPosition(positionFloat);

			// update rotation
			float rotation = XMVectorGetX(XMVector2AngleBetweenNormals(XMLoadFloat2(&ZeroAngleVector), direction));
			if (XMVectorGetY(direction) < 0)
			{
				rotation = -rotation;
			}
			transform.SetRotation(rotation);

			sprite->SetTransform(transform);
		}
	}
}
