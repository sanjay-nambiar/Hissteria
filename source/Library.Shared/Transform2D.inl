#pragma once

namespace DX
{
	inline const DirectX::XMFLOAT2& Transform2D::Position() const
	{
		return mPosition;
	}

	inline void Transform2D::SetPosition(const DirectX::XMFLOAT2& position)
	{
		mPosition = position;
	}

	inline void Transform2D::SetPosition(const float x, const float y)
	{
		mPosition = DirectX::XMFLOAT2(x, y);
	}

	inline DirectX::XMMATRIX Transform2D::TranslationMatrix() const
	{
		return DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, 0.0f);
	}

	inline float Transform2D::Rotation() const
	{
		return mRotation;
	}

	inline void Transform2D::SetRotation(const float rotation)
	{
		mRotation = rotation;
	}

	inline DirectX::XMMATRIX Transform2D::RotationMatrix() const
	{
		return DirectX::XMMatrixRotationZ(mRotation);
	}

	inline const DirectX::XMFLOAT2& Transform2D::Scale() const
	{
		return mScale;
	}

	inline DirectX::XMMATRIX Transform2D::ScalingMatrix() const
	{
		return DirectX::XMMatrixScaling(mScale.x, mScale.y, 1.0f);
	}

	inline void Transform2D::SetScale(const DirectX::XMFLOAT2& scale)
	{
		mScale = scale;
	}

	inline DirectX::XMMATRIX Transform2D::WorldMatrix() const
	{
		return ScalingMatrix() * RotationMatrix() * TranslationMatrix();
	}
}