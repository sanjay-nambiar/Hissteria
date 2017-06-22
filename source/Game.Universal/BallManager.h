#pragma once

#include "DrawableGameComponent.h"
#include <DirectXMath.h>
#include <vector>

namespace DirectXGame
{
	class Ball;
	class Field;

	class BallManager final : public DX::DrawableGameComponent
	{
	public:
		BallManager(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<DX::Camera>& camera);

		std::shared_ptr<Field> ActiveField() const;
		void SetActiveField(const std::shared_ptr<Field>& field);

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

	private:
		void InitializeLineVertices();
		void InitializeTriangleVertices();
		void InitializeBalls();
		void DrawBall(const Ball& ball);
		void DrawSolidBall(const Ball& ball);

		static const std::uint32_t CircleResolution;
		static const std::uint32_t LineCircleVertexCount;
		static const std::uint32_t SolidCircleVertexCount;

		Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mLineVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mTriangleVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVSCBufferPerObject;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPSCBufferPerObject;
		bool mLoadingComplete;
		std::vector<std::shared_ptr<Ball>> mBalls;
		std::shared_ptr<Field> mActiveField;
	};
}

