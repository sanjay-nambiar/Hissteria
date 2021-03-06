#pragma once

#include "MatrixHelper.h"
#include <cstdint>

namespace DX
{
	class TextRenderer;
	enum class TextRenderer::AnchorPoint;
}

namespace DirectXGame
{
	class GameMain;
	class Snake;
	enum class Snake::SnakeType;

	class ProgramHelper
	{
	public:
		struct PlayerConfig
		{
			std::string mName;
			std::uint32_t mBlocks;
			DirectX::XMFLOAT2 mDimension;
			DirectX::XMFLOAT2 mPosition;
			DirectX::XMFLOAT2 mHeading;
			Snake::SnakeType mType;
			DirectX::XMFLOAT4 mHeadColor;
			DirectX::XMFLOAT4 mBodyColor;
			DX::TextRenderer::AnchorPoint mScoreAnchorPoint;
		};

		static const DirectX::XMFLOAT2 RandomLocationInsideBorder(const DirectX::XMFLOAT2& dimension);
		static float RandomFloat(float min, float max);
		static int32_t RandomInt(std::int32_t min, std::int32_t max);
		static void ToWideString(const std::string& source, std::wstring& dest);
		static std::wstring ToWideString(const std::string& source);
		static void Totring(const std::wstring& source, std::string& dest);
		static std::string ToString(const std::wstring& source);

		static const float Left;
		static const float Right;
		static const float Top;
		static const float Bottom;
		static const std::vector<PlayerConfig> PlayerConfigs;

		static bool IsDebugEnabled;
	private:
		static std::default_random_engine RandomGenerator;
		static std::uniform_real_distribution<float> RandomRealDistribution;
		static std::uniform_int_distribution<std::uint32_t> RandomIntDistribution;

		friend GameMain;
	};
}
