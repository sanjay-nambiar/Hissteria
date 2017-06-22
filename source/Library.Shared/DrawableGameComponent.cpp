#include "pch.h"
#include "DrawableGameComponent.h"

using namespace std;

namespace DX
{
	DrawableGameComponent::DrawableGameComponent(const shared_ptr<DX::DeviceResources>& deviceResources) :
		GameComponent(deviceResources),
		mVisible(true)
	{
	}

	DrawableGameComponent::DrawableGameComponent(const shared_ptr<DX::DeviceResources>& deviceResources, const shared_ptr<Camera>& camera) :
		GameComponent(deviceResources),
		mVisible(true), mCamera(camera)
	{
	}

	bool DrawableGameComponent::Visible() const
	{
		return mVisible;
	}

	void DrawableGameComponent::SetVisible(bool visible)
	{
		mVisible = visible;
	}

	shared_ptr<Camera> DrawableGameComponent::GetCamera()
	{
		return mCamera;
	}

	void DrawableGameComponent::SetCamera(const std::shared_ptr<Camera>& camera)
	{
		mCamera = camera;
	}

	void DrawableGameComponent::Render(const StepTimer& timer)
	{
		UNREFERENCED_PARAMETER(timer);
	}
}