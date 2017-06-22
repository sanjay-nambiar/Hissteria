#pragma once

#include "DrawableGameComponent.h"
#include "StepTimer.h"
#include <string>

namespace DX
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class FpsTextRenderer final : public DrawableGameComponent
	{
	public:
		FpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		
		virtual void CreateDeviceDependentResources() override;
		virtual void ReleaseDeviceDependentResources() override;
		virtual void Update(const DX::StepTimer& timer) override;
		virtual void Render(const DX::StepTimer& timer) override;

	private:
		std::wstring                                    m_text;
		DWRITE_TEXT_METRICS	                            m_textMetrics;
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>    m_whiteBrush;
		Microsoft::WRL::ComPtr<ID2D1DrawingStateBlock1> m_stateBlock;
		Microsoft::WRL::ComPtr<IDWriteTextLayout3>      m_textLayout;
		Microsoft::WRL::ComPtr<IDWriteTextFormat2>      m_textFormat;
	};
}