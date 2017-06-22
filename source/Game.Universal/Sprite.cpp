#include "pch.h"
#include "Sprite.h"

using namespace DX;
using namespace DirectX;

namespace DirectXGame
{
	Sprite::Sprite(const DirectX::XMINT2& spriteIndex, const Transform2D& transform, const XMFLOAT4X4& textureTransform) :
		mSpriteIndex(spriteIndex), mTransform(transform), mTextureTransform(textureTransform), mColor(1.0f, 1.0f, 1.0f, 1.0f)
	{
	}

	const DirectX::XMINT2& Sprite::SpriteIndex() const
	{
		return mSpriteIndex;
	}

	void Sprite::SetSpriteIndex(const DirectX::XMINT2& spriteIndex)
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

	const DirectX::XMFLOAT4& Sprite::Color() const
	{
		return mColor;
	}

	void Sprite::SetColor(const DirectX::XMFLOAT4& color)
	{
		mColor = color;
	}
}
