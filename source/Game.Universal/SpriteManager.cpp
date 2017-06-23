#include "pch.h"
#include "SpriteManager.h"

using namespace std;
using namespace DX;
using namespace DirectX;
using namespace Microsoft::WRL;

namespace DirectXGame
{
	const DirectX::XMFLOAT2 SpriteManager::SpriteScale = XMFLOAT2(3.0f, 3.0f);
	const uint32_t SpriteManager::SpriteColumns = 3;
	const uint32_t SpriteManager::SpriteRows = 1;
	const XMFLOAT2 SpriteManager::UVScalingFactor = XMFLOAT2(1.0f / SpriteColumns, 1.0f / SpriteRows);
	const std::wstring SpriteManager::SpriteSheet = L"Content\\Textures\\Shapes.png";
	const std::wstring SpriteManager::SpriteRendererVS = L"SpriteRendererVS.cso";
	const std::wstring SpriteManager::SpriteRendererPS = L"SpriteRendererPS.cso";

	SpriteManager::SpriteManager(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera) :
		DrawableGameComponent(deviceResources, camera),
		mLoadingComplete(false), mIndexCount(0), mRandomRealDistribution(-45.0f, 45.0f), mRandomIntDistribution(0, 3),
		mRandomGenerator(static_cast<uint32_t>(chrono::system_clock::now().time_since_epoch().count()))
	{
	}

	void SpriteManager::CreateDeviceDependentResources()
	{
		auto loadVSTask = ReadDataAsync(SpriteRendererVS);
		auto loadPSTask = ReadDataAsync(SpriteRendererPS);

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

			CD3D11_BUFFER_DESC vsConstantBufferDesc(sizeof(VSCBufferPerObject), D3D11_BIND_CONSTANT_BUFFER);
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateBuffer(
					&vsConstantBufferDesc,
					nullptr,
					mVSCBufferPerObject.ReleaseAndGetAddressOf()
				)
			);

			CD3D11_BUFFER_DESC psConstantBufferDesc(sizeof(PSCBufferPerObject), D3D11_BIND_CONSTANT_BUFFER);
			ThrowIfFailed(
				mDeviceResources->GetD3DDevice()->CreateBuffer(
					&psConstantBufferDesc,
					nullptr,
					mPSCBufferPerObject.ReleaseAndGetAddressOf()
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
			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			ThrowIfFailed(mDeviceResources->GetD3DDevice()->CreateBlendState(&blendStateDesc, mAlphaBlending.ReleaseAndGetAddressOf()));
		});

		auto loadSpriteSheetAndCreateSpritesTask = (createPSTask && createVSTask).then([this]() {
			ThrowIfFailed(CreateWICTextureFromFile(mDeviceResources->GetD3DDevice(), SpriteSheet.c_str(), nullptr, mSpriteSheet.ReleaseAndGetAddressOf()));
			InitializeVertices();
		});

		// Once the cube is loaded, the object is ready to be rendered.
		loadSpriteSheetAndCreateSpritesTask.then([this]() {
			mLoadingComplete = true;
		});
	}

	void SpriteManager::ReleaseDeviceDependentResources()
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

	void SpriteManager::Update(const StepTimer&)
	{
		if (!mLoadingComplete)
		{
			return;
		}
	}

	void SpriteManager::Render(const StepTimer&)
	{
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
		direct3DDeviceContext->PSSetConstantBuffers(0, 1, mPSCBufferPerObject.GetAddressOf());
		direct3DDeviceContext->PSSetShaderResources(0, 1, mSpriteSheet.GetAddressOf());
		direct3DDeviceContext->PSSetSamplers(0, 1, mTextureSampler.GetAddressOf());
		direct3DDeviceContext->OMSetBlendState(mAlphaBlending.Get(), 0, 0xFFFFFFFF);

		for (const auto& sprite : mSprites)
		{
			DrawSprite(*sprite);
		}
	}

	std::weak_ptr<Sprite> SpriteManager::CreateSprite(const DirectX::XMINT2& spriteIndex, const DX::Transform2D& transform)
	{
		auto sprite = make_shared<Sprite>(spriteIndex);

		XMFLOAT4X4 textureTransform;
		XMMATRIX textureTransformMatrix = XMMatrixScaling(UVScalingFactor.x, UVScalingFactor.y, 0) * XMMatrixTranslation(UVScalingFactor.x * sprite->SpriteIndex().x,
			UVScalingFactor.y * sprite->SpriteIndex().y, 0.0f);
		XMStoreFloat4x4(&textureTransform, textureTransformMatrix);
		sprite->SetTextureTransform(textureTransform);
		sprite->SetTransform(transform);
		
		mSprites.push_back(move(sprite));
		return mSprites.back();
	}

	void SpriteManager::RemoveSprite(const std::weak_ptr<Sprite>& sprite)
	{
		mSprites.erase(std::remove(mSprites.begin(), mSprites.end(), static_cast<std::shared_ptr<Sprite>>(sprite)), mSprites.end());
	}

	void SpriteManager::DrawSprite(Sprite& sprite)
	{
		ID3D11DeviceContext* direct3DDeviceContext = mDeviceResources->GetD3DDeviceContext();

		const XMMATRIX wvp = XMMatrixTranspose(sprite.Transform().WorldMatrix() * mCamera->ViewProjectionMatrix());
		XMStoreFloat4x4(&mVSCBufferPerObjectData.WorldViewProjection, wvp);
		XMMATRIX textureTransform = XMLoadFloat4x4(&sprite.TextureTransform());
		XMStoreFloat4x4(&mVSCBufferPerObjectData.TextureTransform, XMMatrixTranspose(textureTransform));
		direct3DDeviceContext->UpdateSubresource(mVSCBufferPerObject.Get(), 0, nullptr, &mVSCBufferPerObjectData, 0, 0);

		mPSCBufferPerObjectData.TintColor = sprite.Color();
		direct3DDeviceContext->UpdateSubresource(mPSCBufferPerObject.Get(), 0, nullptr, &mPSCBufferPerObjectData, 0, 0);
		direct3DDeviceContext->DrawIndexed(mIndexCount, 0, 0);
	}

	void SpriteManager::InitializeVertices()
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
}
