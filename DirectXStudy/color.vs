cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


struct VetexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInput 
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelInput VS(VetexInput in)
{
	PixelInput out;

	in.position.w = 1.f;

	out.position = mul(in.position, wolrdMatrix);
	out.position = mul(out.position, viewMatrix);
	out.position = mul(out.position, projectionMatrix);

	out.color = in.color;
	return out;

}