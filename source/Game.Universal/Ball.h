#pragma once

#include "Transform2D.h"
#include "VectorHelper.h"
#include <DirectXMath.h>
#include <DirectXColors.h>

namespace DX
{
	class StepTimer;
}

namespace DirectXGame
{
	class BallManager;

	class Ball final
	{
	public:
		Ball(BallManager& ballManager, const DX::Transform2D& transform, float radius, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(&DirectX::Colors::White[0]), const DirectX::XMFLOAT2& velocity = DX::Vector2Helper::Zero, bool isSolid = false);
		Ball(const Ball&) = default;
		Ball& operator=(const Ball&) = delete;
		Ball(Ball&&) = default;
		Ball& operator=(Ball&&) = default;
		~Ball() = default;

		const DX::Transform2D& Transform() const;
		void SetTransform(const DX::Transform2D& transform);

		float Radius() const;
		void SetRadius(const float radius);

		const DirectX::XMFLOAT4& Color() const;
		void SetColor(const DirectX::XMFLOAT4& color);

		const DirectX::XMFLOAT2& Velocity() const;
		void SetVelocity(const DirectX::XMFLOAT2& velocity);

		bool IsSolid() const;
		void SetIsSolid(const bool isSolid);

		void Update(const DX::StepTimer& timer);

	private:
		void CheckForFieldCollision();

		BallManager& mBallManager;
		DX::Transform2D mTransform;
		float mRadius;
		DirectX::XMFLOAT4 mColor;
		DirectX::XMFLOAT2 mVelocity;
		bool mIsSolid;
	};
}