#pragma once

#include "VectorHelper.h"
#include <DirectXMath.h>

namespace DX
{
	class Transform2D final
	{
	public:
		static const Transform2D Identity;

		Transform2D(const DirectX::XMFLOAT2& position = Vector2Helper::Zero, float rotation = 0.0f, const DirectX::XMFLOAT2& scale = Vector2Helper::One);

		const DirectX::XMFLOAT2& Position() const;
		void SetPosition(const DirectX::XMFLOAT2& position);
		void SetPosition(const float x, const float y);
		DirectX::XMMATRIX TranslationMatrix() const;

		float Rotation() const;
		void SetRotation(const float rotation);
		DirectX::XMMATRIX RotationMatrix() const;

		const DirectX::XMFLOAT2& Scale() const;
		DirectX::XMMATRIX ScalingMatrix() const;
		void SetScale(const DirectX::XMFLOAT2& scale);

		DirectX::XMMATRIX WorldMatrix() const;

	private:
		DirectX::XMFLOAT2 mPosition;
		float mRotation;
		DirectX::XMFLOAT2 mScale;
	};
}

#include "Transform2D.inl"