#pragma once

#include "DrawableGameComponent.h"
#include <DirectXMath.h>
#include <vector>

namespace DX
{
	struct VertexPosition;
}

namespace DirectXGame
{
	class Field;

	class FieldManager final : public DX::DrawableGameComponent
	{
	public:
		FieldManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera);
				
		std::shared_ptr<Field> ActiveField() const;
		void SetActiveField(const std::shared_ptr<Field>& field);

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Render(const DX::StepTimer& timer) override;

	private:
		void DrawField(const Field& field);

		static const std::uint32_t CircleResolution;
		static const std::uint32_t CircleVertexCount;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSCBufferPerObject;
		bool mLoadingComplete;
		std::uint32_t mIndexCount;
		std::shared_ptr<Field> mActiveField;
	};
}