#include "pch.h"
#include "TimerComponent.h"

using namespace std;
using namespace DX;

namespace DirectXGame
{
	const std::uint32_t TimerComponent::MaxTimers = 1000;

	TimerComponent::CallbackData::CallbackData(const CallbackSignature& callback, void* data, float timePeriod, uint32_t loopCount) :
		mCallback(callback), mData(data), mTimePeriod(timePeriod), mLoopCount(loopCount),
		mCurrentTime(0.0f), mCurrentLoopCount(0), mIsAlive(true)
	{
	}

	TimerComponent::CallbackData::CallbackData(CallbackData&& rhs) :
		mCallback(rhs.mCallback), mData(rhs.mData), mTimePeriod(rhs.mTimePeriod), mLoopCount(rhs.mLoopCount),
		mCurrentTime(rhs.mCurrentTime), mCurrentLoopCount(rhs.mCurrentLoopCount), mIsAlive(rhs.mIsAlive)
	{
	}

	TimerComponent::CallbackData& TimerComponent::CallbackData::operator=(CallbackData&& rhs)
	{
		mCallback = rhs.mCallback;
		mData = rhs.mData;
		mTimePeriod = rhs.mTimePeriod;
		mLoopCount = rhs.mLoopCount;
		mCurrentTime = rhs.mCurrentTime;
		mCurrentLoopCount = rhs.mCurrentLoopCount;
		mIsAlive = rhs.mIsAlive;

		rhs.mIsAlive = false;
		return *this;
	}

	TimerComponent::TimerComponent(const shared_ptr<DX::DeviceResources>& deviceResources) :
		GameComponent(deviceResources)
	{
		mTimers.reserve(MaxTimers);
	}

	void TimerComponent::Update(const StepTimer& gameTime)
	{
		float deltaTime = static_cast<float>(gameTime.GetElapsedSeconds());
		for (std::uint32_t index = 0; index < mTimers.size(); ++index)
		{
			auto& timer = mTimers[index];
			timer.mCurrentTime += deltaTime;
			if (timer.mCurrentTime >= timer.mTimePeriod)
			{
				timer.mCurrentTime = 0;
				++timer.mCurrentLoopCount;
				timer.mCallback(timer.mData);

				if (timer.mCurrentLoopCount == timer.mLoopCount)
				{
					timer.mIsAlive = false;
				}
			}
		}

		const auto& eraseIt = std::partition(mTimers.begin(), mTimers.end(), [] (const CallbackData& timer) {
			return timer.mIsAlive;
		});
		mTimers.erase(eraseIt, mTimers.end());
	}

	void TimerComponent::AddTimer(const CallbackSignature& callback, void* data, float timePeriod, uint32_t loopCount)
	{
		if (loopCount == 0)
		{
			loopCount = 0xFFFFFFFF;
		}
		mTimers.push_back(CallbackData(callback, data, timePeriod, loopCount));
	}
}
