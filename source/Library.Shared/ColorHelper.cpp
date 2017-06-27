#include "pch.h"
#include "ColorHelper.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

namespace DX
{
	const XMFLOAT4 ColorHelper::White() { return { 1.0f, 1.0f, 1.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Gray() { return { 0.5f, 0.5f, 0.5f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Black() { return { 0.0f, 0.0f, 0.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Red() { return { 1.0f, 0.0f, 0.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Maroon() { return { 0.5f, 0.0f, 0.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Yellow() { return { 1.0f, 1.0f, 0.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Olive() { return { 0.5f, 0.5f, 0.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Lime() { return { 0.0f, 1.0f, 0.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Green() { return { 0.0f, 0.5f, 0.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Aqua() { return { 0.0f, 1.0f, 1.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Teal() { return { 0.0f, 0.5f, 0.5f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Blue() { return { 0.0f, 0.0f, 1.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Navy() { return { 0.0f, 0.0f, 0.5f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Fuchsia() { return { 1.0f, 0.0f, 1.0f, 1.0f }; }
	const XMFLOAT4 ColorHelper::Purple() { return { 0.5f, 0.0f, 0.5f, 1.0f }; }

	std::random_device ColorHelper::sDevice;
	std::default_random_engine ColorHelper::sGenerator(sDevice());
	std::uniform_real_distribution<float> ColorHelper::sDistribution(0, 1);

	XMFLOAT4 ColorHelper::RandomColor()
	{
		float r = sDistribution(sGenerator);
		float g = sDistribution(sGenerator);
		float b = sDistribution(sGenerator);

		return XMFLOAT4(r, g, b, 1.0f);
	}

	XMFLOAT4 ColorHelper::ToFloat4(const XMCOLOR& color, bool normalize)
	{
		return (normalize ? XMFLOAT4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f) : XMFLOAT4(color.r, color.g, color.b, color.a));
	}
}
