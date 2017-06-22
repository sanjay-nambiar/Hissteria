#pragma once

#include "GameComponent.h"
#include <memory>

namespace DX
{
    class Camera;

    class DrawableGameComponent : public GameComponent
    {
    public:
		DrawableGameComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		DrawableGameComponent(const std::shared_ptr<DX::DeviceResources>& deviceResources, const std::shared_ptr<Camera>& camera);
		DrawableGameComponent(const DrawableGameComponent&) = default;
		DrawableGameComponent& operator=(const DrawableGameComponent&) = default;
		DrawableGameComponent(DrawableGameComponent&&) = default;
		DrawableGameComponent& operator=(DrawableGameComponent&&) = default;
		virtual ~DrawableGameComponent() = default;		

        bool Visible() const;
        void SetVisible(bool visible);

		std::shared_ptr<Camera> GetCamera();
		void SetCamera(const std::shared_ptr<Camera>& camera);

        virtual void Render(const DX::StepTimer& timer);

    protected:
        bool mVisible;
		std::shared_ptr<Camera> mCamera;
    };
}