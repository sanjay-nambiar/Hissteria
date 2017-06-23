#pragma once

#include "DrawableGameComponent.h"
#include "MatrixHelper.h"
#include <chrono>
#include <memory>
#include <random>
#include <vector>

namespace DirectXGame
{
	class Sprite;

	class SpriteManager final : public DX::DrawableGameComponent
	{
	public:
		SpriteManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera);

		void CreateDeviceDependentResources() override;
		void ReleaseDeviceDependentResources() override;
		void Update(const DX::StepTimer& timer) override;
		void Render(const DX::StepTimer& timer) override;
		
		std::weak_ptr<Sprite> CreateSprite(const DirectX::XMINT2& spriteIndex, const DX::Transform2D& transform = DX::Transform2D());
		void RemoveSprite(const std::weak_ptr<Sprite>& sprite);

	private:
		struct VSCBufferPerObject
		{
			DirectX::XMFLOAT4X4 WorldViewProjection;
			DirectX::XMFLOAT4X4 TextureTransform;

			VSCBufferPerObject() :
				WorldViewProjection(DX::MatrixHelper::Identity), TextureTransform(DX::MatrixHelper::Identity)
			{ };

			VSCBufferPerObject(const DirectX::XMFLOAT4X4& wvp, const DirectX::XMFLOAT4X4& textureTransform) :
				WorldViewProjection(wvp), TextureTransform(textureTransform)
			{ }
		};

		struct PSCBufferPerObject
		{
			DirectX::XMFLOAT4 TintColor;

			PSCBufferPerObject() :
				TintColor(1.0f, 1.0f, 1.0f, 1.0f)
			{ };

			PSCBufferPerObject(const DirectX::XMFLOAT4& tintColor) :
				TintColor(tintColor)
			{ }
		};

		void DrawSprite(Sprite& sprite);
		void InitializeVertices();

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSpriteSheet;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mTextureSampler;
		Microsoft::WRL::ComPtr<ID3D11BlendState> mAlphaBlending;

		VSCBufferPerObject mVSCBufferPerObjectData;
		PSCBufferPerObject mPSCBufferPerObjectData;
		bool mLoadingComplete;
		std::vector<std::shared_ptr<Sprite>> mSprites;
		std::uint32_t mIndexCount;

		std::default_random_engine mRandomGenerator;
		std::uniform_real_distribution<float> mRandomRealDistribution;
		std::uniform_int_distribution<std::uint32_t> mRandomIntDistribution;

		static const DirectX::XMFLOAT2 SpriteScale;
		static const std::uint32_t SpriteColumns;
		static const std::uint32_t SpriteRows;
		static const DirectX::XMFLOAT2 UVScalingFactor;
		static const std::wstring SpriteSheet;
		static const std::wstring SpriteRendererVS;
		static const std::wstring SpriteRendererPS;
	};
}
