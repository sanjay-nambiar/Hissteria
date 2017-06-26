#pragma once

#include "DrawableGameComponent.h"
#include "StepTimer.h"
#include <string>

namespace DX
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class TextRenderer : public DrawableGameComponent
	{
	public:
		enum class AnchorPoint
		{
			TopLeft,
			Top,
			TopRight,
			Left,
			Center,
			Right,
			BottomLeft,
			Bottom,
			BottomRight
		};

		TextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Render(const DX::StepTimer& timer) override;

		void SetText(const std::wstring& text, std::uint32_t maxWidth, std::uint32_t maxHeight);
		void SetFont(const std::wstring& fontName, float fontSize = 32.0f);
		void SetTextFormatting(const DirectX::XMFLOAT4& color, AnchorPoint anchorPoint);

		static const std::wstring DefaultFont;
	private:
		std::wstring                                    mText;
		DWRITE_TEXT_METRICS	                            mTextMetrics;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    mColoredBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> mStateBlock;
		Microsoft::WRL::ComPtr<IDWriteTextLayout3>      mTextLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>      mTextFormat;

		AnchorPoint mAnchorPoint;
		std::wstring mFontName;
		float mFontSize;
		DirectX::XMFLOAT4 mColor;
	};
}
