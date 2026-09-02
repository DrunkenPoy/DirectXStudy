#include "LightShader.h"

CLightShader::CLightShader() {
  m_vertexShader = nullptr;
  m_pixelShader = nullptr;
  m_layout = nullptr;
  m_sampleState = nullptr;
  m_matrixBuffer = nullptr;
  m_lightBuffer = nullptr;
}

CLightShader::CLightShader(const CLightShader &other) {
  m_vertexShader = other.m_vertexShader;
  m_pixelShader = other.m_pixelShader;
  m_layout = other.m_layout;
  m_sampleState = other.m_sampleState;
  m_matrixBuffer = other.m_matrixBuffer;
  m_lightBuffer = other.m_lightBuffer;
}

CLightShader::~CLightShader() {}

bool CLightShader::Initialize(ID3D11Device *pDevice, HWND hwnd) 
{
	wchar_t vsFilename[128];
	wchar_t psFilename[128];

	if (wcscpy_s(vsFilename, 128, L"../DirectXStudy/light.vs"))
		return false;

	if (wcscpy_s(psFilename, 128, L"../DirectXStudy/light.ps"))
		return false;

  return !InitializeShader(pDevice, hwnd, vsFilename, psFilename);
}

void CLightShader::Shutdown() 
{
	ShutdownShader();
	return;
}

bool CLightShader::Render(ID3D11DeviceContext *pDeviceContext, int indexCount,
						  XMMATRIX worldMatrix, XMMATRIX viewMatrix,
						  XMMATRIX projectionMatrix,
						  ID3D11ShaderResourceView *texture,
						  XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) 
{
	if (!SetShaderParameters(pDeviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor))
		return false;
	RenderShader(pDeviceContext, indexCount);
	return true;
}

bool CLightShader::InitializeShader(ID3D11Device *pDevice, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename) 
{

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	uint numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;


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

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;


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
	FAILED_CHECK_RETURN(pDevice->CreateSamplerState(&samplerDesc, &m_sampleState), false);

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	FAILED_CHECK_RETURN(pDevice->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer), false);


	return true;
}

void CLightShader::ShutdownShader() 
{
	ReleaseCOM_Ptr(m_lightBuffer);
	ReleaseCOM_Ptr(m_sampleState);
	ReleaseCOM_Ptr(m_matrixBuffer);
	ReleaseCOM_Ptr(m_layout);
	ReleaseCOM_Ptr(m_pixelShader);
	ReleaseCOM_Ptr(m_vertexShader);

	return;
}

void CLightShader::OutputShaderErrorMessage(ID3D10Blob *errorMessage, HWND hwnd, WCHAR *shaderFilename) 
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

bool CLightShader::SetShaderParameters(ID3D11DeviceContext *pDeviceContext,
										XMMATRIX worldMatrix, XMMATRIX viewMatrix,
										XMMATRIX projectionMatrix,
										ID3D11ShaderResourceView *texture,
										XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) 
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	UINT bufferNumber;


	//셰이더에 넘기기위한 행렬 전치
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//상수 버퍼에 사용할 수 있도록 잠금
	FAILED_CHECK_RETURN(pDeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource), false);

	//상수 버퍼 내부의 데이터 포인터를 얻음.
	dataPtr = (MatrixBuffer*)mappedResource.pData;

	//행렬들을 상수 버퍼에 복사
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//상수 버퍼 잠금 해제
	pDeviceContext->Unmap(m_matrixBuffer, 0);

	//정점 셰이더에서 상수 버퍼의 위치 설정
	bufferNumber = 0;

	//마지막으로 갱싱된 값으로 정점 셰이더의 상수 버퍼를 설정
	pDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//픽셀셰이더에 셰이더 텍스처 리소스를 설정
	pDeviceContext->PSSetShaderResources(0, 1, &texture);

	//조명 상수 버퍼에 사용할 수 있도록 잠금
	FAILED_CHECK_RETURN(pDeviceContext->Map(m_lightBuffer,0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

	//상수 버퍼 내부 데이터의 포인터를 얻음.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	//조명 변수들을 상수 버퍼에 복사
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	//상수 버퍼 잠금 해제
	pDeviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	pDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void CLightShader::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount) 
{
	//정점 입력 레이아웃을 설정한다.
	pDeviceContext->IASetInputLayout(m_layout);

	//이 삼각형을 렌더링하는 데 사용할 셰이더 설정함.
	pDeviceContext->VSSetShader(m_vertexShader, NULL, 0);
	pDeviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//샘플러 상태 지정
	pDeviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//렌더링
	pDeviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}