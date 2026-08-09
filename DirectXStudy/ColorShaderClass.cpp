#include "ColorShaderClass.h"


ColorShaderClass::ColorShaderClass()
{

}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device*, HWND)
{
	return false;
}

bool ColorShaderClass::Render(ID3D11DeviceContext*, int, XMMATRIX, XMMATRIX, XMMATRIX)
{
	return false;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* pDevice, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	uint numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


	//버텍스 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(vsFilename,
		NULL, NULL,
		"ColorVertexShader", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, 
		&vertexShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//픽셀 셰이더 컴파일
	if (FAILED(D3DCompileFromFile(psFilename,
		NULL, NULL,
		"ColorPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage)))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//정점셰이더 생성
	FAILED_CHECK_RETURN(pDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader), false);
	//픽셀셰이더 생성
	FAILED_CHECK_RETURN(pDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader), false);



	return false;
}

void ColorShaderClass::ShutdownShader()
{
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*)
{
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext*, XMMATRIX, XMMATRIX, XMMATRIX)
{
	return false;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext*, int)
{
}
