#include "pch.h"
#include "Sprite.h"

using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	Sprite::Sprite(uint32_t spriteId, const Transform2D & transform, const XMFLOAT4X4& textureTransform) :
		mSpriteId(spriteId), mTransform(transform), mTextureTransform(textureTransform)
	{
	}

	uint32_t Sprite::SpriteIndex() const
	{
		return mSpriteId;
	}

	void Sprite::SetSpriteIndex(const uint32_t spriteId)
	{
		mSpriteId = spriteId;
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
}
