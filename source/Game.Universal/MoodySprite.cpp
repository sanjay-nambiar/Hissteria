#include "pch.h"
#include "MoodySprite.h"

using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	MoodySprite::MoodySprite(uint32_t spriteIndex, const Transform2D & transform, Moods mood, const XMFLOAT4X4& textureTransform) :
		mSpriteIndex(spriteIndex), mTransform(transform), mMood(mood), mTextureTransform(textureTransform)
	{
	}

	uint32_t MoodySprite::SpriteIndex() const
	{
		return mSpriteIndex;
	}

	void MoodySprite::SetSpriteIndex(const uint32_t spriteIndex)
	{
		mSpriteIndex = spriteIndex;
	}

	const Transform2D& MoodySprite::Transform() const
	{
		return mTransform;
	}

	void MoodySprite::SetTransform(const Transform2D& transform)
	{
		mTransform = transform;
	}

	MoodySprite::Moods MoodySprite::Mood() const
	{
		return mMood;
	}

	void MoodySprite::SetMood(const Moods mood)
	{
		mMood = mood;
	}

	const XMFLOAT4X4& MoodySprite::TextureTransform() const
	{
		return mTextureTransform;
	}

	void MoodySprite::SetTextureTransform(const XMFLOAT4X4& transform)
	{
		mTextureTransform = transform;
	}
}