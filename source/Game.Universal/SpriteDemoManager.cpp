#include "pch.h"
#include "SpriteDemoManager.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Microsoft::WRL;

namespace DirectXGame
{
	const DirectX::XMFLOAT2 SpriteDemoManager::SpriteScale = XMFLOAT2(3.0f, 3.0f);
	const uint32_t SpriteDemoManager::SpriteCount = 8; // Sprites are arranged horizontally within the sprite sheet
	const uint32_t SpriteDemoManager::MoodCount = 4; // Moods are arranged vertically within the sprite sheet
	const XMFLOAT2 SpriteDemoManager::UVScalingFactor = XMFLOAT2(1.0f / SpriteCount, 1.0f / MoodCount);
	const double SpriteDemoManager::MoodUpdateDelay = 0.5; // Delay between mood changes, in seconds

	SpriteDemoManager::SpriteDemoManager(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera, uint32_t spriteRowCount, uint32_t spriteColumCount) :
		DrawableGameComponent(deviceResources, camera),
		mLoadingComplete(false), mIndexCount(0),
		mSpriteRowCount(spriteRowCount), mSpriteColumnCount(spriteColumCount),
		mPosition(0.0f, 0.0f), mRandomGenerator(mRandomDevice()),
		mSpriteDistribution(0, SpriteCount - 1)
	{
	}

	const XMFLOAT2& SpriteDemoManager::Position() const
	{
		return mPosition;
	}

	void SpriteDemoManager::SetPositon(const XMFLOAT2& position)
	{
		mPosition = position;
	}

	void SpriteDemoManager::CreateDeviceDependentResources()
	{
		auto loadVSTask = ReadDataAsync(L"SpriteRendererVS.cso");
		auto loadPSTask = ReadDataAsync(L"SpriteRendererPS.cso");

		// After the vertex shader file is loaded, create the shader and input layout.
		auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateVertexShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					mVertexShader.ReleaseAndGetAddressOf()
				)
			);

			// Create an input layout
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateInputLayout(
					VertexPositionTexture::InputElements,
					VertexPositionTexture::InputElementCount,
					&fileData[0],
					fileData.size(),
					mInputLayout.ReleaseAndGetAddressOf()
				)
			);

			CD3D11_BUFFER_DESC constantBufferDesc(sizeof(VSCBufferPerObject), D3D11_BIND_CONSTANT_BUFFER);
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					mVSCBufferPerObject.ReleaseAndGetAddressOf()
				)
			);
		});

		// After the pixel shader file is loaded, create the shader and texture sampler state.
		auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreatePixelShader(
					&fileData[0],
					fileData.size(),
					nullptr,
					mPixelShader.ReleaseAndGetAddressOf()
				)
			);

			D3D11_SAMPLER_DESC samplerStateDesc;
			ZeroMemory(&samplerStateDesc, sizeof(samplerStateDesc));
			samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			samplerStateDesc.MinLOD = -FLT_MAX;
			samplerStateDesc.MaxLOD = FLT_MAX;
			samplerStateDesc.MipLODBias = 0.0f;
			samplerStateDesc.MaxAnisotropy = 1;
			samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateSamplerState(&samplerStateDesc, mTextureSampler.ReleaseAndGetAddressOf()));

			D3D11_BLEND_DESC blendStateDesc = { 0 };
			blendStateDesc.RenderTarget[0].BlendEnable = true;
			blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBlendState(&blendStateDesc, mAlphaBlending.ReleaseAndGetAddressOf()));
		});

		auto loadSpriteSheetAndCreateSpritesTask = (createPSTask && createVSTask).then([this]() {
			ThrowIfFailed(CreateWICTextureFromFile(mDeviceResources->GetD3DDevice(), L"Content\\Textures\\snoods_default.png", nullptr, mSpriteSheet.ReleaseAndGetAddressOf()));			
			InitializeVertices();
			InitializeSprites();
		});

		// Once the cube is loaded, the object is ready to be rendered.
		loadSpriteSheetAndCreateSpritesTask.then([this]() {
			mLoadingComplete = true;
			mSpriteCountDistribution = uniform_int_distribution<uint32_t>(0U, static_cast<uint32_t>(mSprites.size()) - 1);
		});
	}

	void SpriteDemoManager::ReleaseDeviceDependentResources()
	{
		mLoadingComplete = false;
		mVertexShader.Reset();
		mPixelShader.Reset();
		mInputLayout.Reset();
		mVertexBuffer.Reset();
		mIndexBuffer.Reset();
		mVSCBufferPerObject.Reset();
		mSpriteSheet.Reset();
		mTextureSampler.Reset();
	}

	void SpriteDemoManager::Update(const StepTimer& timer)
	{
		if (!mLoadingComplete)
		{
			return;
		}

		if (timer.GetTotalSeconds() > mLastMoodUpdateTime + MoodUpdateDelay)
		{
			mLastMoodUpdateTime = timer.GetTotalSeconds();

			uint32_t spritesToChange = mSpriteCountDistribution(mRandomGenerator);
			for (uint32_t i = 0; i < spritesToChange; ++i)
			{
				uint32_t spriteIndex = mSpriteCountDistribution(mRandomGenerator);
				auto sprite = mSprites[spriteIndex];
				ChangeMood(*sprite);
			}
		}
	}

	void SpriteDemoManager::Render(const StepTimer & timer)
	{
		UNREFERENCED_PARAMETER(timer);

		// Loading is asynchronous. Only draw geometry after it's loaded.
		if (!mLoadingComplete)
		{
			return;
		}

		ID3D11DeviceContext* direct3DDeviceContext = mDeviceResources->GetD3DDeviceContext();
		direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		direct3DDeviceContext->IASetInputLayout(mInputLayout.Get());

		static const UINT stride = sizeof(VertexPositionTexture);
		static const UINT offset = 0;
		direct3DDeviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		direct3DDeviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
		direct3DDeviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);
		direct3DDeviceContext->VSSetConstantBuffers(0, 1, mVSCBufferPerObject.GetAddressOf());
		direct3DDeviceContext->PSSetShaderResources(0, 1, mSpriteSheet.GetAddressOf());
		direct3DDeviceContext->PSSetSamplers(0, 1, mTextureSampler.GetAddressOf());
		direct3DDeviceContext->OMSetBlendState(mAlphaBlending.Get(), 0, 0xFFFFFFFF);

		for (const auto& sprite : mSprites)
		{
			DrawSprite(*sprite);
		}
	}

	void SpriteDemoManager::DrawSprite(MoodySprite& sprite)
	{
		ID3D11DeviceContext* direct3DDeviceContext = mDeviceResources->GetD3DDeviceContext();
		
		const XMMATRIX wvp = XMMatrixTranspose(sprite.Transform().WorldMatrix() * mCamera->ViewProjectionMatrix());
		XMStoreFloat4x4(&mVSCBufferPerObjectData.WorldViewProjection, wvp);
		XMMATRIX textureTransform = XMLoadFloat4x4(&sprite.TextureTransform());
		XMStoreFloat4x4(&mVSCBufferPerObjectData.TextureTransform, XMMatrixTranspose(textureTransform));		 
		direct3DDeviceContext->UpdateSubresource(mVSCBufferPerObject.Get(), 0, nullptr, &mVSCBufferPerObjectData, 0, 0);

		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}

	void SpriteDemoManager::InitializeVertices()
	{
		VertexPositionTexture vertices[] = 
		{
			VertexPositionTexture(XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)),
			VertexPositionTexture(XMFLOAT4(-1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)),
			VertexPositionTexture(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)),
			VertexPositionTexture(XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)),
		};

		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexPositionTexture) * ARRAYSIZE(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexSubResourceData = { 0 };
		vertexSubResourceData.pSysMem = vertices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, mVertexBuffer.ReleaseAndGetAddressOf()));

		// Create and index buffer
		const uint32_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		mIndexCount = ARRAYSIZE(indices);

		D3D11_BUFFER_DESC indexBufferDesc = { 0 };
		indexBufferDesc.ByteWidth = sizeof(uint32_t) * mIndexCount;
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexSubResourceData = { 0 };
		indexSubResourceData.pSysMem = indices;
		ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexSubResourceData, mIndexBuffer.ReleaseAndGetAddressOf()));
	}

	void SpriteDemoManager::InitializeSprites()
	{	
		const XMFLOAT2 neighborOffset(2.0f, 2.0f);
		for (uint32_t column = 0; column < mSpriteColumnCount; ++column)		
		{
			for (uint32_t row = 0; row < mSpriteRowCount; ++row)
			{
				XMFLOAT2 position(mPosition.x + column * neighborOffset.x * SpriteScale.x, mPosition.y + row * neighborOffset.y * SpriteScale.y);
				Transform2D transform(position, 0.0f, SpriteScale);								
				uint32_t spriteIndex = mSpriteDistribution(mRandomGenerator);
				auto sprite = make_shared<MoodySprite>(spriteIndex, transform);
				ChangeMood(*sprite);
				mSprites.push_back(move(sprite));
			}
		}
	}

	void SpriteDemoManager::ChangeMood(MoodySprite& sprite)
	{
		MoodySprite::Moods mood = GetRandomMood();

		XMFLOAT4X4 textureTransform;
		XMMATRIX textureTransformMatrix = XMMatrixScaling(UVScalingFactor.x, UVScalingFactor.y, 0) * XMMatrixTranslation(UVScalingFactor.x * sprite.SpriteIndex(), UVScalingFactor.y * static_cast<uint32_t>(mood), 0.0f);
		XMStoreFloat4x4(&textureTransform, textureTransformMatrix);
		sprite.SetTextureTransform(textureTransform);
	}

	MoodySprite::Moods SpriteDemoManager::GetRandomMood()
	{
		static uniform_int_distribution<uint32_t> moodDistribution(static_cast<uint32_t>(MoodySprite::Moods::Neutral), static_cast<uint32_t>(MoodySprite::Moods::Angry));

		return static_cast<MoodySprite::Moods>(moodDistribution(mRandomGenerator));
	}
}