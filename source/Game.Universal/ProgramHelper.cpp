#include "pch.h"
#include "ProgramHelper.h"

using namespace DirectX;
using namespace DX;
using namespace std;
using namespace std::chrono;

namespace DirectXGame
{
	bool ProgramHelper::IsDebugEnabled = false;

	const float ProgramHelper::Left = -50.0f;
	const float ProgramHelper::Right = 50.0f;
	const float ProgramHelper::Top = 50.0f;
	const float ProgramHelper::Bottom = -50.0f;

	const vector<ProgramHelper::PlayerConfig> ProgramHelper::PlayerConfigs =
	{
		// Player 1
		{
			3,
			{ 6.0f, 3.0f },
			{ -50.0f, 40.0f },
			{ 1.0f, 0.0f },
			Snake::SnakeType::ChainLink,
			ColorHelper::Blue(),
			ColorHelper::Green(),
		},

		// Player 2
		{
			3,
			{ 6.0f, 3.0f },
			{ 50.0f, -40.0f },
			{ -1.0f, 0.0f },
			Snake::SnakeType::ChainLink,
			ColorHelper::Purple(),
			ColorHelper::White(),
		}
	};

	default_random_engine ProgramHelper::RandomGenerator = default_random_engine(static_cast<uint32_t>(system_clock::now().time_since_epoch().count()));
	uniform_real_distribution<float> ProgramHelper::RandomRealDistribution = uniform_real_distribution<float>(0, 1000.0f);
	uniform_int_distribution<uint32_t> ProgramHelper::RandomIntDistribution = uniform_int_distribution<uint32_t>(0, 1000);

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
		float random = (RandomRealDistribution(RandomGenerator) / 1000) * (max - min);
		return (random + min);
	}

	int32_t ProgramHelper::RandomInt(std::int32_t min, std::int32_t max)
	{
		return (RandomIntDistribution(RandomGenerator) % (max - min)) + min;
	}
}
