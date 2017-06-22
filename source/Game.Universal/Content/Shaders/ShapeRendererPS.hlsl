cbuffer CBufferPerObject
{
	float4 Color;
}

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
};

float4 main(VS_OUTPUT IN) : SV_TARGET
{
	return Color;
}