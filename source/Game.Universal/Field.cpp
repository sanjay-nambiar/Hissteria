#include "pch.h"
#include "Field.h"

using namespace DirectX;

namespace DirectXGame
{
	Field::Field(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color) :
		mPosition(position), mSize(size), mColor(color)
	{
	}

	const XMFLOAT2 & Field::Position() const
	{
		return mPosition;
	}

	void Field::SetPosition(const XMFLOAT2& position)
	{
		mPosition = position;
	}

	const XMFLOAT2& Field::Size() const
	{
		return mSize;
	}

	void Field::SetSize(const XMFLOAT2& size)
	{
		mSize = size;
	}

	const XMFLOAT4& Field::Color() const
	{
		return mColor;
	}

	void Field::SetColor(const XMFLOAT4& color)
	{
		mColor = color;
	}
}