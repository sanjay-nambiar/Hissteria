#include "pch.h"
#include "FpsTextRenderer.h"
#include "DirectXHelper.h"

using namespace Microsoft::WRL;

namespace DX
{
	// Initializes D2D resources used for text rendering.
	FpsTextRenderer::FpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		DrawableGameComponent(deviceResources),
		m_text(L"")
	{
		ZeroMemory(&m_textMetrics, sizeof(DWRITE_TEXT_METRICS));

		// Create device independent resources
		ComPtr<IDWriteTextFormat> textFormat;
		DX::ThrowIfFailed(
			mDeviceResources->GetDWriteFactory()->CreateTextFormat(
				L"Segoe UI",
				nullptr,
				DWRITE_FONT_WEIGHT_LIGHT,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				32.0f,
				L"en-US",
				&textFormat
			)
		);

		DX::ThrowIfFailed(
			textFormat.As(&m_textFormat)
		);

		DX::ThrowIfFailed(
			m_textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);

		DX::ThrowIfFailed(
			mDeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&m_stateBlock)
		);

		CreateDeviceDependentResources();
	}

	// Updates the text to be displayed.
	void FpsTextRenderer::Update(const StepTimer& timer)
	{
		// Update display text.
		uint32 fps = timer.GetFramesPerSecond();

		m_text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";

		ComPtr<IDWriteTextLayout> textLayout;
		DX::ThrowIfFailed(
			mDeviceResources->GetDWriteFactory()->CreateTextLayout(
				m_text.c_str(),
				(uint32)m_text.length(),
				m_textFormat.Get(),
				240.0f, // Max width of the input text.
				50.0f, // Max height of the input text.
				&textLayout
			)
		);

		DX::ThrowIfFailed(
			textLayout.As(&m_textLayout)
		);

		DX::ThrowIfFailed(
			m_textLayout->GetMetrics(&m_textMetrics)
		);
	}

	// Renders a frame to the screen.
	void FpsTextRenderer::Render(const StepTimer& timer)
	{
		UNREFERENCED_PARAMETER(timer);

		ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();
		Windows::Foundation::Size logicalSize = mDeviceResources->GetLogicalSize();

		context->SaveDrawingState(m_stateBlock.Get());
		context->BeginDraw();

		// Position on the bottom right corner
		D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
			logicalSize.Width - m_textMetrics.layoutWidth,
			logicalSize.Height - m_textMetrics.height
		);

		context->SetTransform(screenTranslation * mDeviceResources->GetOrientationTransform2D());

		DX::ThrowIfFailed(
			m_textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
		);

		context->DrawTextLayout(
			D2D1::Point2F(0.f, 0.f),
			m_textLayout.Get(),
			m_whiteBrush.Get()
		);

		// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
		// is lost. It will be handled during the next call to Present.
		HRESULT hr = context->EndDraw();
		if (hr != D2DERR_RECREATE_TARGET)
		{
			DX::ThrowIfFailed(hr);
		}

		context->RestoreDrawingState(m_stateBlock.Get());
	}

	void FpsTextRenderer::CreateDeviceDependentResources()
	{
		DX::ThrowIfFailed(
			mDeviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_whiteBrush)
		);
	}
	void FpsTextRenderer::ReleaseDeviceDependentResources()
	{
		m_whiteBrush.Reset();
	}
}