#include "pch.h"
#include "GameComponent.h"
#include "StepTimer.h"

using namespace std;

namespace DX
{
	GameComponent::GameComponent(const shared_ptr<DX::DeviceResources>& deviceResources) :
		mDeviceResources(deviceResources), mEnabled(true)
	{
	}

	shared_ptr<DeviceResources> GameComponent::DeviceResources() const
	{
		return mDeviceResources;
	}

	bool GameComponent::Enabled() const
	{
		return mEnabled;
	}

	void GameComponent::SetEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	void GameComponent::CreateDeviceDependentResources()
	{
	}

	void GameComponent::CreateWindowSizeDependentResources()
	{
	}

	void GameComponent::ReleaseDeviceDependentResources()
	{
	}
	
	void GameComponent::Update(const StepTimer& timer)
	{
		UNREFERENCED_PARAMETER(timer);
	}
}