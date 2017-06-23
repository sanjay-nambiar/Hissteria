#pragma once

namespace DirectXGame
{
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
	private:
		DirectX::XMFLOAT4X4 mTextureTransform;
		DX::Transform2D mTransform;
		DirectX::XMINT2 mSpriteIndex;
		DirectX::XMFLOAT4 mColor;
	};
}
