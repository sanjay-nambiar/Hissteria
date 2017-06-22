#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <random>

namespace DX
{
	class ColorHelper
	{
	public:
		static DirectX::XMFLOAT4 RandomColor();
		static DirectX::XMFLOAT4 ToFloat4(const DirectX::PackedVector::XMCOLOR& color, bool normalize = false);

		ColorHelper() = delete;
		ColorHelper(const ColorHelper&) = delete;
		ColorHelper(ColorHelper&&) = delete;
		ColorHelper& operator=(const ColorHelper&) = delete;
		ColorHelper& operator=(ColorHelper&&) = delete;

	private:
		static std::random_device sDevice;
		static std::default_random_engine sGenerator;
		static std::uniform_real_distribution<float> sDistribution;
	};
}