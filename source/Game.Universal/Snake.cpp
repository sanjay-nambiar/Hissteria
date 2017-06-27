#include "pch.h"
#include "Snake.h"

using namespace DirectX;
using namespace DX;
using namespace std;

namespace DirectXGame
{
	const std::uint32_t Snake::MaxBodyBlocks = 15;
	const std::uint32_t Snake::DefaultHealth = 10;
	const float Snake::MaxSpeed = 50.0f;
	const float Snake::MaxForce = 5.0f;
	
	const float Snake::BlinkForwardTime = 0.2f;
	const float Snake::BlinkBackwardTime = 0.2f;
	const uint32_t Snake::BlinkCount = 3;

	const XMFLOAT2 Snake::BlockScale = { 3.0f, 3.0f };
	const XMFLOAT2 Snake::ZeroAngleVector = { 1.0f, 0.0f };

	const std::unordered_map<Snake::SnakeType, Snake::SnakeTypeConfig> Snake::SnakeTypeConfigMapping = {
		{ SnakeType::Circular, { XMINT2(0, 0), 3.0f } },
		{ SnakeType::ChainLink, { XMINT2(1, 0), 3.0f } }
	};

	Snake::Snake(std::uint32_t id, SnakeType type, uint32_t bodyBlocks, const XMFLOAT2& blockDimension, const XMFLOAT2& position, const XMFLOAT2& heading,
		const XMFLOAT4& headColor, const XMFLOAT4& bodyColor, const shared_ptr<SpriteManager>& spriteManager) :
		mDimension(blockDimension), mHeadingDirection(heading), mHealth(DefaultHealth), mIsInvincible(false), mIsAlive(true),
		mSpeed(MaxSpeed), mColliderRadius(SnakeTypeConfigMapping.at(type).mColliderRadius), mId(id), mType(type), mScore(0),
		mSpriteManager(spriteManager), mBlockSeparation(0.0f), mHeadColor(headColor), mBodyColor(bodyColor)
	{
		assert(bodyBlocks > 0 && bodyBlocks < MaxBodyBlocks);
		mBody.reserve(bodyBlocks);

		XMFLOAT2 positionFloat = position;
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
		sprite->SetColor(mHeadColor);

		auto transform = Transform2D();
		XMVECTOR positionVector = XMLoadFloat2(&positionFloat);

		float rotation = XMVectorGetX(XMVector2AngleBetweenNormals(XMLoadFloat2(&ZeroAngleVector), XMLoadFloat2(&mHeadingDirection)));
		if (mHeadingDirection.y < 0)
		{
			rotation = -rotation;
		}
		transform.SetRotation(rotation);

		XMStoreFloat2(&positionFloat, positionVector);
		transform.SetPosition(positionFloat);
		transform.SetScale(BlockScale);
		sprite->SetTransform(transform);
		mBody.emplace_back(block);

		AddBlocks(bodyBlocks - 1);
	}

	bool Snake::AddBlocks(std::uint32_t blocks)
	{
		bool blockAdded = false;
		for (std::uint32_t n = 0; n < blocks; ++n)
		{
			if (mBody.size() >= MaxBodyBlocks)
			{
				return false;
			}

			BodyBlock block;
			block.mSprite = mSpriteManager->CreateSprite(SnakeTypeConfigMapping.at(mType).mSpriteIndex);
			auto sprite = block.mSprite.lock();
			sprite->SetColor(mBodyColor);

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
			blockAdded = true;
		}

		return blockAdded;
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

	void Snake::BlinkSnake(const DirectX::XMFLOAT4& mBlinkColor, BlinkStyle blinkStyle)
	{
		if (blinkStyle != BlinkStyle::TailOnly)
		{
			mBody.front().mSprite.lock()->SetColorInterpolation(mBlinkColor, BlinkForwardTime, BlinkBackwardTime, BlinkCount);
		}

		if (blinkStyle == BlinkStyle::FullBody)
		{
			for (std::uint32_t index = 1; index < mBody.size() - 1; ++index)
			{
				auto& block = mBody[index];
				block.mSprite.lock()->SetColorInterpolation(mBlinkColor, BlinkForwardTime, BlinkBackwardTime, BlinkCount);
			}
		}

		if (blinkStyle != BlinkStyle::HeadOnly)
		{
			mBody.back().mSprite.lock()->SetColorInterpolation(mBlinkColor, BlinkForwardTime, BlinkBackwardTime, BlinkCount);
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
		if (!mIsAlive)
		{
			return;
		}

		XMVECTOR velocity = XMVector2Normalize(XMLoadFloat2(&mHeadingDirection)) * mSpeed;
		XMVECTOR prevPosition;
		XMVECTOR prevBlockNewPosition;
		float deltaPositionLength;

		float width = ProgramHelper::Right - ProgramHelper::Left;
		float height = ProgramHelper::Top - ProgramHelper::Bottom;
		float minX = width * 100;
		float maxX = -minX;
		float minY = height * 100;
		float maxY = -minY;

		{
			// Update head
			auto& head = mBody.front();
			auto sprite = head.mSprite.lock();
			auto transform = sprite->Transform();

			XMVECTOR position = XMLoadFloat2(&transform.Position());
			prevPosition = position;
			position = position + (velocity * static_cast<float>(timer.GetElapsedSeconds()));

			// wrapping
			float x = XMVectorGetX(position);
			float y = XMVectorGetY(position);
			if (x < minX)
			{
				minX = x;
			}
			if (x > maxX)
			{
				maxX = x;
			}

			if (y < minY)
			{
				minY = y;
			}
			if (y > maxY)
			{
				maxY = y;
			}

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

			// wrapping
			float x = XMVectorGetX(position);
			float y = XMVectorGetY(position);
			if (x < minX)
			{
				minX = x;
			}
			else if (x > maxX)
			{
				maxX = x;
			}

			if (y < minY)
			{
				minY = y;
			}
			else if (y > maxY)
			{
				maxY = y;
			}

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

		XMFLOAT2 adjustment = { 0.0f, 0.0f };
		if ((minX - mColliderRadius) > ProgramHelper::Right)
		{
			adjustment.x = -(width + (maxX - ProgramHelper::Right));
		}
		else if ((maxX + mColliderRadius) < ProgramHelper::Left)
		{
			adjustment.x = (width + (ProgramHelper::Left - minX));
		}

		if ((minY - mColliderRadius) > ProgramHelper::Top)
		{
			adjustment.y = -(height + (maxY - ProgramHelper::Top));
		}
		else if ((maxY + mColliderRadius) < ProgramHelper::Bottom)
		{
			adjustment.y = (height + (ProgramHelper::Bottom - minY));
		}

		XMVECTOR adjustMentVector = XMLoadFloat2(&adjustment);
		for (auto& block : mBody)
		{
			const auto& sprite = block.mSprite.lock();
			auto transform = sprite->Transform();
			XMFLOAT2 mirroredPos;
			XMStoreFloat2(&mirroredPos, adjustMentVector + XMLoadFloat2(&transform.Position()));
			transform.SetPosition(mirroredPos);
			sprite->SetTransform(transform);
		}
	}

	void Snake::Hurt()
	{
		if (mHealth > 0)
		{
			--mHealth;
		}
	}

	void Snake::Kill()
	{
		mIsAlive = false;
		for (auto& block : mBody)
		{
			block.mSprite.lock()->SetIsVisible(false);
		}
	}

	void Snake::Revive(const XMFLOAT2& position, uint32_t blocks)
	{
		mSpeed = Snake::MaxSpeed;
		mIsInvincible = false;
		mIsAlive = true;
		mHealth = DefaultHealth;
		mScore = 0;

		const auto& headSprite = mBody.front().mSprite.lock();
		auto transform = headSprite->Transform();
		headSprite->SetIsVisible(true);
		transform.SetPosition(position);

		float rotation = XMVectorGetX(XMVector2AngleBetweenNormals(XMLoadFloat2(&ZeroAngleVector), XMLoadFloat2(&mHeadingDirection)));
		if (mHeadingDirection.y < 0)
		{
			rotation = -rotation;
		}
		transform.SetRotation(rotation);
		headSprite->SetTransform(transform);

		ShrinkSnake(1);
		AddBlocks(blocks - 1);
	}

	bool Snake::CheckCollisionWithSnake(const std::shared_ptr<Snake>& snake)
	{
		const auto& headSprite = mBody.front().mSprite.lock();
		const auto position = XMLoadFloat2(&headSprite->Transform().Position());
		float distance = (snake->ColliderRadius() + mColliderRadius);
		float otherRadiusSq = snake->ColliderRadius();
		otherRadiusSq *= otherRadiusSq;

		// if checking collision with self, start at index 1, else start at index 0
		std::uint32_t index = (snake.get() != this) ? 0 : 1;
		for (; index < snake->mBody.size(); ++index)
		{
			const auto& body = snake->mBody[index];
			XMVECTOR otherPosition = XMLoadFloat2(&body.mSprite.lock()->Transform().Position());
			float length = XMVectorGetX(XMVector2Length(position - otherPosition));
			if (distance >= length)
			{
				XMVECTOR testPoint = position + (XMLoadFloat2(&mHeadingDirection) * length);
				if (XMVectorGetX(XMVector2LengthSq(testPoint - otherPosition)) < otherRadiusSq)
				{
					return true;
				}
			}
		}
		return false;
	}
}
