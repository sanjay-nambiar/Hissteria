#include "pch.h"
#include "TextRenderer.h"
#include "DirectXHelper.h"
#include "ColorHelper.h"

using namespace Microsoft::WRL;
using namespace std;

namespace DX
{
	const std::wstring TextRenderer::DefaultFont = L"Segoe UI";

	// Initializes D2D resources used for text rendering.
	TextRenderer::TextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		DrawableGameComponent(deviceResources),
		mText(L""), mAnchorPoint(AnchorPoint::TopLeft), mFontName(L""), mFontSize(32.0f)
	{
		ZeroMemory(&mTextMetrics, sizeof(DWRITE_TEXT_METRICS));
		SetFont(DefaultFont);
		SetTextFormatting(ColorHelper::White(), AnchorPoint::BottomRight);
	}

	void TextRenderer::SetText(const std::wstring& text, std::uint32_t maxWidth, std::uint32_t maxHeight)
	{
		mText = text;

		ComPtr<IDWriteTextLayout> textLayout;
		DX::ThrowIfFailed(
			mDeviceResources->GetDWriteFactory()->CreateTextLayout(
				mText.c_str(),
				(uint32)mText.length(),
				mTextFormat.Get(),
				static_cast<float>(maxWidth),
				static_cast<float>(maxHeight),
				&textLayout
			)
		);

		DX::ThrowIfFailed(
			textLayout.As(&mTextLayout)
		);

		DX::ThrowIfFailed(
			mTextLayout->GetMetrics(&mTextMetrics)
		);
	}

	void TextRenderer::SetFont(const std::wstring& fontName, const float fontSize)
	{
		mFontName = fontName;
		mFontSize = fontSize;
		
		ComPtr<IDWriteTextFormat> textFormat;
		DX::ThrowIfFailed(
			mDeviceResources->GetDWriteFactory()->CreateTextFormat(
				mFontName.c_str(),
				nullptr,
				DWRITE_FONT_WEIGHT_LIGHT,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				mFontSize,
				L"en-US",
				&textFormat
			)
		);

		DX::ThrowIfFailed(
			textFormat.As(&mTextFormat)
		);

		DX::ThrowIfFailed(
			mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
		);

		DX::ThrowIfFailed(
			mDeviceResources->GetD2DFactory()->CreateDrawingStateBlock(&mStateBlock)
		);
	}

	void TextRenderer::SetTextFormatting(const DirectX::XMFLOAT4& color, AnchorPoint anchorPoint)
	{
		mAnchorPoint = anchorPoint;
		mColor = color;
		CreateDeviceDependentResources();
	}

	// Renders a frame to the screen.
	void TextRenderer::Render(const StepTimer&)
	{
		ID2D1DeviceContext* context = mDeviceResources->GetD2DDeviceContext();
		Windows::Foundation::Size logicalSize = mDeviceResources->GetLogicalSize();

		context->SaveDrawingState(mStateBlock.Get());
		context->BeginDraw();

		const float left = -350;
		const float center = (logicalSize.Width / 2) - (mTextMetrics.layoutWidth / 2) + 20;
		const float right = logicalSize.Width - mTextMetrics.layoutWidth - 50;
		const float top = 0;
		const float middle = (logicalSize.Height / 2) - (mTextMetrics.layoutHeight / 2) - 40;
		const float bottom = logicalSize.Height - mTextMetrics.height;

		const std::unordered_map<AnchorPoint, D2D1::Matrix3x2F> textAlignments =
		{
			{ AnchorPoint::TopLeft, D2D1::Matrix3x2F::Translation(left, top) },
			{ AnchorPoint::Top, D2D1::Matrix3x2F::Translation(center, top) },
			{ AnchorPoint::TopRight, D2D1::Matrix3x2F::Translation(right, top) },

			{ AnchorPoint::Left, D2D1::Matrix3x2F::Translation(left, middle) },
			{ AnchorPoint::Center, D2D1::Matrix3x2F::Translation(center, middle) },
			{ AnchorPoint::Right, D2D1::Matrix3x2F::Translation(right, middle) },
			
			{ AnchorPoint::BottomLeft, D2D1::Matrix3x2F::Translation(left, bottom) },
			{ AnchorPoint::Bottom, D2D1::Matrix3x2F::Translation(center, bottom) },
			{ AnchorPoint::BottomRight, D2D1::Matrix3x2F::Translation(right, bottom) },
		};
		context->SetTransform(textAlignments.at(mAnchorPoint) * mDeviceResources->GetOrientationTransform2D());

		DX::ThrowIfFailed(
			mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING)
		);

		context->DrawTextLayout(
			D2D1::Point2F(0.f, 0.f),
			mTextLayout.Get(),
			mColoredBrush.Get()
		);

		// Ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
		// is lost. It will be handled during the next call to Present.
		HRESULT hr = context->EndDraw();
		if (hr != D2DERR_RECREATE_TARGET)
		{
			DX::ThrowIfFailed(hr);
		}

		context->RestoreDrawingState(mStateBlock.Get());
	}

	void TextRenderer::CreateDeviceDependentResources()
	{
		DX::ThrowIfFailed(
			mDeviceResources->GetD2DDeviceContext()->CreateSolidColorBrush(D2D1::ColorF(mColor.x, mColor.y, mColor.z), &mColoredBrush)
		);
	}
	
	void TextRenderer::ReleaseDeviceDependentResources()
	{
		mColoredBrush.Reset();
	}
}
