#pragma once

#include "DeviceResources.h"
#include <memory>

namespace DX
{
	class StepTimer;

	class GameComponent
	{
	public:
		GameComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		GameComponent(const GameComponent&) = default;
		GameComponent& operator=(const GameComponent&) = default;
		GameComponent(GameComponent&&) = default;
		GameComponent& operator=(GameComponent&&) = default;
		virtual ~GameComponent() = default;

		std::shared_ptr<DX::DeviceResources> DeviceResources() const;
		bool Enabled() const;
		void SetEnabled(bool enabled);

		virtual void CreateDeviceDependentResources();
		virtual void CreateWindowSizeDependentResources();
		virtual void ReleaseDeviceDependentResources();

		virtual void Update(const StepTimer& timer);

	protected:
		std::shared_ptr<DX::DeviceResources> mDeviceResources;
		bool mEnabled;
	};
}
