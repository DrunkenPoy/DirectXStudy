#include "TextureShader.h"

CTextureShader::CTextureShader() 
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
}

CTextureShader::CTextureShader(const CTextureShader &other) 
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
}

CTextureShader::~CTextureShader() 
{

}

bool CTextureShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128];
	wchar_t psFilename[128];


	if (wcscpy_s(vsFilename, 128, L"../DirectXStudy/texture.vs") != 0)
		return false;
	if (wcscpy_s(psFilename, 128, L"../DirectXStudy/texture.ps") != 0)
		return false;
	if (!InitializeShader(device, hwnd, vsFilename, psFilename))
		return false;

	return true;
}

void CTextureShader::Shutdown()
{
	ShutdownShader();
	return;
}

bool CTextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
		return false;
	RenderShader(deviceContext, indexCount);

	return true;
}

bool CTextureShader::InitializeShader(ID3D11Device* pDevice, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	uint numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	//버텍스 쉐이더 컴파일
	if (FAILED(D3DCompileFromFile(vsFilename,
		NULL, NULL,
		"VS", "vs_5_0",
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
		"PS", "ps_5_0",
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


	//정점 입력 레이아웃 설명 구조체를 생성
	// 이 설정은 ModelClass와 셰이더의 VertexType구조체와 일치해야함.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//레이아웃의 요소 개수를 구한다.
	numElements = sizeof(polygonLayout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	//정점 입력 레이아웃 생성
	FAILED_CHECK_RETURN(pDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout), false);

	ReleaseCOM_Ptr(vertexShaderBuffer);
	ReleaseCOM_Ptr(pixelShaderBuffer);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	FAILED_CHECK_RETURN(pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer), false);

	//텍스처 샘플러 상태 설명 구조체를 생성한다.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//텍스처 샘플러 생성
	FAILED_CHECK_RETURN(pDevice->CreateSamplerState(&samplerDesc,&m_sampleState), false);


	return true;
}

void CTextureShader::ShutdownShader()
{
	ReleaseCOM_Ptr(m_sampleState);
	ReleaseCOM_Ptr(m_matrixBuffer);
	ReleaseCOM_Ptr(m_layout);
	ReleaseCOM_Ptr(m_pixelShader);
	ReleaseCOM_Ptr(m_vertexShader);

	return;
}

void CTextureShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	ULONGLONG bufferSize;
	ofstream fout;

	compileErrors = (char*)(errorMessage->GetBufferPointer());

	bufferSize = errorMessage->GetBufferSize();

	fout.open("shder-error.txt");

	for (ULONGLONG i = 0; i < bufferSize; ++i)
		fout << compileErrors[i];
	fout.close();

	ReleaseCOM_Ptr(errorMessage);

	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.",
		shaderFilename, MB_OK);
	return;
}

bool CTextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	return true;
}

void CTextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
}

