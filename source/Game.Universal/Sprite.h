#pragma once

namespace DX
{
	class StepTimer;
}

namespace DirectXGame
{
	class SpriteManager;

	class Sprite
	{
	public:
		Sprite(const DirectX::XMINT2& spriteIndex, const DX::Transform2D& transform = DX::Transform2D(),
			const DirectX::XMFLOAT4X4& textureTransform = DX::MatrixHelper::Identity);

		const DirectX::XMINT2& SpriteIndex() const;
		void SetSpriteIndex(const DirectX::XMINT2& spriteIndex);

		const DX::Transform2D& Transform() const;
		void SetTransform(const DX::Transform2D& transform);

		const DirectX::XMFLOAT4X4& TextureTransform() const;
		void SetTextureTransform(const DirectX::XMFLOAT4X4& transform);

		const DirectX::XMFLOAT4& Color() const;
		void SetColor(const DirectX::XMFLOAT4& color);

		bool IsVisible() const;
		void SetIsVisible(bool isVisible);

		void SetColorInterpolation(const DirectX::XMFLOAT4& color, float interpolationForwardPeriod, float interpolationBackwardPeriod, std::uint32_t interpolationCount);
		void Update(const DX::StepTimer& timer);
	private:
		DirectX::XMFLOAT4X4 mTextureTransform;
		DX::Transform2D mTransform;
		DirectX::XMINT2 mSpriteIndex;
		DirectX::XMFLOAT4 mColor;
		DirectX::XMFLOAT4 mOriginalColor;
		bool mIsVisible;

		DirectX::XMFLOAT4 mInterpolationColor;
		float mInterpolationForwardPeriod;
		float mInterpolationBackwardPeriod;
		std::uint32_t mInterpolationCount;

		float mInterpolationCurrentPeriod;
		float mInterpolationCurrentTimePoint;
		float mInterpolationCurrentCount;
		DirectX::XMFLOAT4 mInterpolationCurrentColor;
		bool mIsForwardInterpolation;

		friend SpriteManager;
	};
}
