
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};


struct VetexInput
{
	float4 position : POSITION;
};

struct PixelInput 
{
	float4 position : SV_POSITION;
	float4 depthPos : TEXCOORD0;
};

PixelInput VS(VetexInput input)
{
	PixelInput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.depthPos = output.position;

	return output;

}