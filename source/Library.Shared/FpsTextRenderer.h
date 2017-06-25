#pragma once

#include "TextRenderer.h"
#include "StepTimer.h"
#include <string>

namespace DX
{
	// Renders the current FPS value in the bottom right corner of the screen using Direct2D and DirectWrite.
	class FpsTextRenderer final : public TextRenderer
	{
	public:
		FpsTextRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);

		virtual void Update(const DX::StepTimer& timer) override;
	};
}