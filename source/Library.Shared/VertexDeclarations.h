#pragma once

#include <DirectXMath.h>
#include <d3d11_2.h>

namespace DX
{
	struct VertexPosition
	{
		VertexPosition() = default;
		
		VertexPosition(const DirectX::XMFLOAT4& position) :
			Position(position) { }

		DirectX::XMFLOAT4 Position;

		static const int InputElementCount = 1;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct VertexPositionColor
	{
		VertexPositionColor() = default;

		VertexPositionColor(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& color) :
			Position(position), Color(color) { }

		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT4 Color;

		static const int InputElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct VertexPositionTexture
	{
		VertexPositionTexture() = default;

		VertexPositionTexture(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates) :
			Position(position), TextureCoordinates(textureCoordinates) { }

		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 TextureCoordinates;

		static const int InputElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct VertexPositionSize
	{
		VertexPositionSize() = default;

		VertexPositionSize(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& size) :
			Position(position), Size(size) { }

		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 Size;

		static const int InputElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct VertexPositionNormal
	{
		VertexPositionNormal() = default;

		VertexPositionNormal(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT3& normal) :
			Position(position), Normal(normal) { }

		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT3 Normal;

		static const int InputElementCount = 2;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct VertexPositionTextureNormal
	{
		VertexPositionTextureNormal() = default;

		VertexPositionTextureNormal(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates, const DirectX::XMFLOAT3& normal) :
			Position(position), TextureCoordinates(textureCoordinates), Normal(normal) { }

		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 TextureCoordinates;
		DirectX::XMFLOAT3 Normal;

		static const int InputElementCount = 3;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct VertexPositionTextureNormalTangent
	{
		VertexPositionTextureNormalTangent() = default;

		VertexPositionTextureNormalTangent(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent) :
			Position(position), TextureCoordinates(textureCoordinates), Normal(normal), Tangent(tangent) { }

		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 TextureCoordinates;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 Tangent;

		static const int InputElementCount = 4;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};

	struct VertexSkinnedPositionTextureNormal
	{
		VertexSkinnedPositionTextureNormal() = default;

		VertexSkinnedPositionTextureNormal(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT2& textureCoordinates, const DirectX::XMFLOAT3& normal, const DirectX::XMUINT4& boneIndices, const DirectX::XMFLOAT4& boneWeights) :
			Position(position), TextureCoordinates(textureCoordinates), Normal(normal), BoneIndices(boneIndices), BoneWeights(boneWeights) { }

		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT2 TextureCoordinates;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMUINT4 BoneIndices;
		DirectX::XMFLOAT4 BoneWeights;

		static const int InputElementCount = 5;
		static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
	};
}
