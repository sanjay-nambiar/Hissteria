#include "pch.h"
#include "ProgramHelper.h"
#include <string>
#include <locale>
#include <codecvt>

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
			"Blue",
			3,
			{ 6.0f, 3.0f },
			{ -20.0f, 40.0f },
			{ 1.0f, 0.0f },
			Snake::SnakeType::ChainLink,
			ColorHelper::Aqua(),
			ColorHelper::Blue(),
			TextRenderer::AnchorPoint::TopLeft
		},

		// Player 2
		{
			"Green",
			3,
			{ 6.0f, 3.0f },
			{ 20.0f, -40.0f },
			{ -1.0f, 0.0f },
			Snake::SnakeType::ChainLink,
			ColorHelper::Lime(),
			ColorHelper::Green(),
			TextRenderer::AnchorPoint::TopRight
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

	void ProgramHelper::ToWideString(const std::string& source, std::wstring& dest)
	{
		dest = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(source);
	}

	std::wstring ProgramHelper::ToWideString(const std::string& source)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(source);
	}

	void ProgramHelper::Totring(const std::wstring& source, std::string& dest)
	{
		dest = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(source);
	}

	std::string ProgramHelper::ToString(const std::wstring& source)
	{
		return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(source);
	}
}
