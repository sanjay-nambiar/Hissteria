#pragma once

#include "Transform2D.h"
#include "MatrixHelper.h"

namespace DirectXGame
{
	class MoodySprite final
	{
	public:
		enum class Moods
		{
			Neutral,
			Happy,
			Surprised,
			Angry
		};

		MoodySprite(std::uint32_t spriteIndex, const DX::Transform2D& transform, Moods mood = Moods::Neutral, const DirectX::XMFLOAT4X4& textureTransform = DX::MatrixHelper::Identity);

		std::uint32_t SpriteIndex() const;
		void SetSpriteIndex(const std::uint32_t spriteIndex);

		const DX::Transform2D& Transform() const;
		void SetTransform(const DX::Transform2D& transform);

		Moods Mood() const;
		void SetMood(const Moods mood);

		const DirectX::XMFLOAT4X4& TextureTransform() const;
		void SetTextureTransform(const DirectX::XMFLOAT4X4& transform);

	private:
		DirectX::XMFLOAT4X4 mTextureTransform;
		DX::Transform2D mTransform;		
		Moods mMood;
		std::uint32_t mSpriteIndex;
	};
}