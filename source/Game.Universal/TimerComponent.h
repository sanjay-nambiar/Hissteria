#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include "GameComponent.h"


namespace DX
{
	class StepTimer;
}

namespace DirectXGame
{
	class TimerComponent final : public DX::GameComponent
	{
	public:
		typedef std::function<void(void*)> CallbackSignature;

		TimerComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		TimerComponent(const TimerComponent&) = default;
		TimerComponent& operator=(const TimerComponent&) = default;
		TimerComponent(TimerComponent&&) = default;
		TimerComponent& operator=(TimerComponent&&) = default;
		~TimerComponent() = default;

		void Update(const DX::StepTimer& gameTime) override;
		void AddTimer(const CallbackSignature& callback, void* data, float timePeriod, std::uint32_t loopCount = 1);
	
	private:

		struct CallbackData
		{
			CallbackSignature mCallback;
			void* mData;
			float mTimePeriod;
			std::uint32_t mLoopCount;
			
			float mCurrentTime;
			std::uint32_t mCurrentLoopCount;
			bool mIsAlive;

			CallbackData(const CallbackSignature& callback, void* data, float timePeriod, std::uint32_t loopCount);
			CallbackData(CallbackData&& rhs);
			CallbackData& operator=(CallbackData&& rhs);
		};

		std::vector<CallbackData> mTimers;
		
		static const std::uint32_t MaxTimers;
	};
}
