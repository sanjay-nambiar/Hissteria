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

	class SpawnManager final : public DX::DrawableGameComponent
	{
	public:
		SpawnManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera, std::uint32_t maxSpawns = 1);

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

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
		void InitializeSprites();

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
		std::chrono::time_point<std::chrono::system_clock> mLastSpawnTime;
		std::uint32_t mMaxSpawns;
		
		static const DirectX::XMFLOAT2 SpriteScale;
		static const std::uint32_t SpriteColumns;
		static const std::uint32_t SpriteRows;
		static const DirectX::XMFLOAT2 UVScalingFactor;
		static const std::uint32_t SpawnLifeTime;
	};
}
