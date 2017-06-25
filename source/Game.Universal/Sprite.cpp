#include "pch.h"
#include "Sprite.h"
#include "StepTimer.h"

using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	Sprite::Sprite(const XMINT2& spriteIndex, const Transform2D& transform, const XMFLOAT4X4& textureTransform) :
		mSpriteIndex(spriteIndex), mTransform(transform), mTextureTransform(textureTransform), mColor(1.0f, 1.0f, 1.0f, 1.0f),
		mInterpolationCurrentPeriod(0.0f), mInterpolationForwardPeriod(0.0f), mInterpolationBackwardPeriod(0.0f), mInterpolationCount(0),
		mInterpolationCurrentTimePoint(0.0f), mInterpolationCurrentCount(0.0f), mIsForwardInterpolation(true)
	{
		mInterpolationColor = mColor;
	}

	const XMINT2& Sprite::SpriteIndex() const
	{
		return mSpriteIndex;
	}

	void Sprite::SetSpriteIndex(const XMINT2& spriteIndex)
	{
		mSpriteIndex = spriteIndex;
	}

	const Transform2D& Sprite::Transform() const
	{
		return mTransform;
	}

	void Sprite::SetTransform(const Transform2D& transform)
	{
		mTransform = transform;
	}

	const XMFLOAT4X4& Sprite::TextureTransform() const
	{
		return mTextureTransform;
	}

	void Sprite::SetTextureTransform(const XMFLOAT4X4& transform)
	{
		mTextureTransform = transform;
	}

	const XMFLOAT4& Sprite::Color() const
	{
		return mColor;
	}

	void Sprite::SetColor(const XMFLOAT4& color)
	{
		mColor = color;
		mOriginalColor = mColor;
	}

	void Sprite::SetColorInterpolation(const XMFLOAT4& color, float interpolationForwardPeriod, float interpolationBackwardPeriod, uint32_t interpolationCount)
	{
		mInterpolationColor = color;
		mInterpolationForwardPeriod = interpolationForwardPeriod;
		mInterpolationBackwardPeriod = interpolationBackwardPeriod;
		mInterpolationCount = interpolationCount;

		mInterpolationCurrentTimePoint = 0.0f;
		mInterpolationCurrentCount = 0;
		mInterpolationCurrentColor = mInterpolationColor;
		mInterpolationCurrentPeriod = mInterpolationForwardPeriod;
		mIsForwardInterpolation = true;
	}

	void Sprite::Update(const StepTimer& timer)
	{
		if (mInterpolationCurrentPeriod > 0.0f)
		{
			mInterpolationCurrentTimePoint += static_cast<float>(timer.GetElapsedSeconds());
			float controlFactor = (mInterpolationCurrentTimePoint / mInterpolationCurrentPeriod);
			XMStoreFloat4(&mColor, XMVectorLerp(XMLoadFloat4(&mColor), XMLoadFloat4(&mInterpolationCurrentColor), controlFactor));

			if (mInterpolationCurrentTimePoint >= mInterpolationCurrentPeriod)
			{
				mInterpolationCurrentTimePoint = 0.0f;
				if (mIsForwardInterpolation)
				{
					mColor = mInterpolationColor;
					mInterpolationCurrentColor = mOriginalColor;
					mInterpolationCurrentPeriod = mInterpolationBackwardPeriod;
					mIsForwardInterpolation = false;
				}
				else
				{
					mColor = mOriginalColor;
					mInterpolationCurrentColor = mInterpolationColor;
					mInterpolationCurrentPeriod = mInterpolationForwardPeriod;
					mIsForwardInterpolation = true;
					++mInterpolationCurrentCount;
				}
			}

			if (mInterpolationCurrentCount >= mInterpolationCount)
			{
				mInterpolationCurrentPeriod = 0.0f;
			}
		}
	}
}
