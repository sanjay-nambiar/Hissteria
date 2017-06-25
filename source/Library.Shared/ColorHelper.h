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

		static const DirectX::XMFLOAT4 White();
		static const DirectX::XMFLOAT4 Gray();
		static const DirectX::XMFLOAT4 Black();
		static const DirectX::XMFLOAT4 Red();
		static const DirectX::XMFLOAT4 Maroon();
		static const DirectX::XMFLOAT4 Yellow();
		static const DirectX::XMFLOAT4 Olive();
		static const DirectX::XMFLOAT4 Lime();
		static const DirectX::XMFLOAT4 Green();
		static const DirectX::XMFLOAT4 Aqua();
		static const DirectX::XMFLOAT4 Teal();
		static const DirectX::XMFLOAT4 Blue();
		static const DirectX::XMFLOAT4 Navy();
		static const DirectX::XMFLOAT4 Fuchsia();
		static const DirectX::XMFLOAT4 Purple();

	private:
		static std::random_device sDevice;
		static std::default_random_engine sGenerator;
		static std::uniform_real_distribution<float> sDistribution;
	};
}