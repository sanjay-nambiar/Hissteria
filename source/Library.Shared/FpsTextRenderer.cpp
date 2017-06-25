#include "pch.h"
#include "FpsTextRenderer.h"
#include "DirectXHelper.h"

using namespace Microsoft::WRL;

namespace DX
{
	// Initializes D2D resources used for text rendering.
	FpsTextRenderer::FpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
		TextRenderer(deviceResources)
	{
	}

	// Updates the text to be displayed.
	void FpsTextRenderer::Update(const StepTimer& timer)
	{
		// Update display text.
		uint32 fps = timer.GetFramesPerSecond();
		std::wstring text = (fps > 0) ? std::to_wstring(fps) + L" FPS" : L" - FPS";
		SetText(text, 240, 50);
	}
}