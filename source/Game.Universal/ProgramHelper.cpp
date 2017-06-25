#include "pch.h"
#include "ProgramHelper.h"

using namespace DirectX;
using namespace std;
using namespace std::chrono;

namespace DirectXGame
{
	bool ProgramHelper::IsDebugEnabled = false;

	const float ProgramHelper::Left = -50.0f;
	const float ProgramHelper::Right = 50.0f;
	const float ProgramHelper::Top = 50.0f;
	const float ProgramHelper::Bottom = -50.0f;

	default_random_engine ProgramHelper::mRandomGenerator = default_random_engine(static_cast<uint32_t>(system_clock::now().time_since_epoch().count()));
	uniform_real_distribution<float> ProgramHelper::mRandomRealDistribution = uniform_real_distribution<float>(0, 1000.0f);
	uniform_int_distribution<uint32_t> ProgramHelper::mRandomIntDistribution = uniform_int_distribution<uint32_t>(0, 1000);

	const XMFLOAT2 ProgramHelper::RandomLocationInsideBorder(const XMFLOAT2& dimension)
	{
		const float left = Left + (dimension.x / 2);
		const float right = Right - (dimension.x / 2);
		const float top = Top - (dimension.y / 2);
		const float bottom = Bottom + (dimension.y / 2);
		return { RandomFloat(left, right), RandomFloat(top, bottom) };
	}

	float ProgramHelper::RandomFloat(float min, float max)
	{
		float random = (mRandomRealDistribution(mRandomGenerator) / 1000) * (max - min);
		return (random + min);
	}

	int32_t ProgramHelper::RandomInt(std::int32_t min, std::int32_t max)
	{
		return (mRandomIntDistribution(mRandomGenerator) % (max - min)) + min;
	}
}
