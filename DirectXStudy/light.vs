
/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 4

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float padding;
};

cbuffer LightPositionBuffer
{
	float4 lightPosition[NUM_LIGHTS];
};

struct VetexInput
{
	float4 position : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInput 
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
	float3 lightPos[NUM_LIGHTS] : TEXCOORD2;
};

PixelInput VS(VetexInput input)
{
	PixelInput output;

	input.position.w = 1.f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.uv = input.uv;
	float4 normal4 = float4(input.normal, 0.0f);

	output.normal = mul(normal4,worldMatrix).xyz;

	output.normal = normalize(output.normal);

	float4 worldPosition = mul(input.position, worldMatrix);

	output.viewDirection = normalize(cameraPosition - worldPosition.xyz);

	worldPosition = mul(input.position, worldMatrix);

	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		//각 광원의 위치와 월드 공간 정점 위치를 이용해 광원 방향을 결정
		output.lightPos[i] = normalize(lightPosition[i].xyz - worldPosition.xyz);
	}


	return output;

}