#include "pch.h"
#include "Transform2D.h"

using namespace DirectX;

namespace DX
{
	const Transform2D Transform2D::Identity = { Vector2Helper::Zero, 0.0f, Vector2Helper::One };

	Transform2D::Transform2D(const XMFLOAT2& position, float rotation, const XMFLOAT2& scale) :
		mPosition(position), mRotation(rotation), mScale(scale)
	{
	}
}