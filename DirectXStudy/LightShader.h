#pragma once
#ifndef __LIGHT_SHADER_H__
#define __LIGHT_SHADER_H__

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "myMacro.h"

/////////////
// GLOBALS //
/////////////
const int NUM_LIGHTS = 4;

using namespace DirectX;
using namespace std;

class CLightShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuseColor[NUM_LIGHTS];
		XMFLOAT3 lightDirection;
		float specularPower;
		XMFLOAT4 specularColor;
	};

	struct LightPositionBufferType
	{
		XMFLOAT4 lightPosition[NUM_LIGHTS];
	};

	struct CameraBufferType
	{
		XMFLOAT3 cameraPosition;
		float padding;  // CreateBuffer 요구사항(16의 배수)에 맞추기 위한 추가 패딩.
	};

	CONSTRUCTION_FEILD_B(CLightShader, public);
public:
	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext* pDeviceContext, int indexCount,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor[],
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, XMFLOAT4 lightposition[]);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext* pDeviceContext,
		XMMATRIX worldMatrix, XMMATRIX viewMatrix,
		XMMATRIX projectionMatrix,
		ID3D11ShaderResourceView* texture,
		XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor[],
		XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower, XMFLOAT4 lightposition[]);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_cameraBuffer;
	ID3D11Buffer* m_lightColorBuffer;
	ID3D11Buffer* m_lightPositionBuffer;
};


#endif __LIGHT_SHADER_H__