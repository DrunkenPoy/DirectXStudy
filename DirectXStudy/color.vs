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

PixelInput VS(VetexInput inV)
{
	PixelInput outP;

	inV.position.w = 1.f;

	outP.position = mul(inV.position, worldMatrix);
	outP.position = mul(outP.position, viewMatrix);
	outP.position = mul(outP.position, projectionMatrix);

	outP.color = inV.color;
	return outP;

}