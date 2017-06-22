#include "pch.h"
#include "Ball.h"

using namespace DirectX;
using namespace DX;

namespace DirectXGame
{
	Ball::Ball(BallManager& ballManager, const Transform2D& transform, float radius, const XMFLOAT4& color, const XMFLOAT2& velocity, bool isSolid) :
		mBallManager(ballManager), mTransform(transform), mRadius(radius),
		mColor(color), mVelocity(velocity), mIsSolid(isSolid)
	{
	}

	const Transform2D& Ball::Transform() const
	{
		return mTransform;
	}

	void Ball::SetTransform(const Transform2D & transform)
	{
		mTransform = transform;
	}

	float Ball::Radius() const
	{
		return mRadius;
	}

	void Ball::SetRadius(const float radius)
	{
		mRadius = radius;
	}

	const XMFLOAT4& Ball::Color() const
	{
		return mColor;
	}

	void Ball::SetColor(const XMFLOAT4& color)
	{
		mColor = color;
	}

	const XMFLOAT2& Ball::Velocity() const
	{
		return mVelocity;
	}

	void Ball::SetVelocity(const XMFLOAT2& velocity)
	{
		mVelocity = velocity;
	}

	bool Ball::IsSolid() const
	{
		return mIsSolid;
	}

	void Ball::SetIsSolid(const bool isSolid)
	{
		mIsSolid = isSolid;
	}

	void Ball::Update(const StepTimer& timer)
	{
		double elapsedTime = timer.GetElapsedSeconds();

		XMFLOAT2 position = mTransform.Position();
		position.x += mVelocity.x * static_cast<float>(elapsedTime);
		position.y += mVelocity.y * static_cast<float>(elapsedTime);

		mTransform.SetPosition(position);
		CheckForFieldCollision();
	}

	void Ball::CheckForFieldCollision()
	{
		auto field = mBallManager.ActiveField();
		const auto& fieldPosition = field->Position();
		const auto& fieldSize = field->Size();
		const XMFLOAT2 fieldHalfSize(fieldSize.x / 2.0f, fieldSize.y / 2.0f);

		const float rightSide = fieldPosition.x + fieldHalfSize.x;
		const float leftSide = fieldPosition.x - fieldHalfSize.x;
		const float topSide = fieldPosition.y + fieldHalfSize.y;
		const float bottomSide = fieldPosition.y - fieldHalfSize.y;

		const auto& position = mTransform.Position();
		XMFLOAT2 updatedPosition = position;

		bool hasCollidedWithField = false;
		if (position.x - mRadius <= leftSide)
		{
			mVelocity.x *= -1;
			updatedPosition.x = leftSide + mRadius;
			hasCollidedWithField = true;
		}
		if (position.x + mRadius >= rightSide)
		{
			mVelocity.x *= -1;
			updatedPosition.x = rightSide - mRadius;
			hasCollidedWithField = true;
		}
		if (position.y - mRadius <= bottomSide)
		{
			mVelocity.y *= -1;
			updatedPosition.y = bottomSide + mRadius;
			hasCollidedWithField = true;
		}
		if (position.y + mRadius >= topSide)
		{
			mVelocity.y *= -1;
			updatedPosition.y = topSide - mRadius;
			hasCollidedWithField = true;
		}

		if (hasCollidedWithField)
		{
			mTransform.SetPosition(updatedPosition);
			mColor = ColorHelper::RandomColor();
		}
	}
}