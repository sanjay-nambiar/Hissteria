#pragma once

#include "MatrixHelper.h"
#include <cstdint>

namespace DirectXGame
{
	class ProgramHelper
	{
	public:
		static const DirectX::XMFLOAT2 RandomLocationInsideBorder(const DirectX::XMFLOAT2& dimension);
		static float RandomFloat(float min, float max);
		static int32_t RandomInt(std::int32_t min, std::int32_t max);

		static const float Left;
		static const float Right;
		static const float Top;
		static const float Bottom;

	private:
		static std::default_random_engine mRandomGenerator;
		static std::uniform_real_distribution<float> mRandomRealDistribution;
		static std::uniform_int_distribution<std::uint32_t> mRandomIntDistribution;
	};
}
