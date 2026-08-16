#include "ColorShaderClass.h"


ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
	m_vertexShader = other.m_vertexShader;
	m_pixelShader = other.m_pixelShader;
	m_layout = other.m_layout;
	m_matrixBuffer = other.m_matrixBuffer;
}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	

	if (wcscpy_s(vsFilename, 128, L"../DirectXStudy/color.vs") != 0)
		return false;
	if (wcscpy_s(psFilename, 128, L"../DirectXStudy/color.ps") != 0)
		return false;
	if (!InitializeShader(device, hwnd, vsFilename, psFilename))
		return false;

	return true;
}

void ColorShaderClass::Shutdown()
{
	ShutdownShader(); return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
		return false;
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* pDevice, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	uint numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;


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
	FAILED_CHECK_RETURN(pDevice->CreateInputLayout(polygonLayout,numElements,vertexShaderBuffer->GetBufferPointer(),vertexShaderBuffer->GetBufferSize(), &m_layout),false);

	ReleaseCOM_Ptr(vertexShaderBuffer);
	ReleaseCOM_Ptr(pixelShaderBuffer);

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	FAILED_CHECK_RETURN(pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer), false);

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	ReleaseCOM_Ptr(m_matrixBuffer);
	ReleaseCOM_Ptr(m_layout);
	ReleaseCOM_Ptr(m_pixelShader);
	ReleaseCOM_Ptr(m_vertexShader);

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
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

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, 
	XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBuffer* dataPtr;
	unsigned int bufferNumber;

	//셰이더에 넘기기위한 행렬 전치
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	//상수 버퍼에 사용할 수 있도록 잠금
	FAILED_CHECK_RETURN(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource), false);

	//상수 버퍼 내부의 데이터 포인터를 얻음.
	dataPtr = (MatrixBuffer*)mappedResource.pData;

	//행렬들을 상수 버퍼에 복사
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	//상수 버퍼 잠금 해제
	deviceContext->Unmap(m_matrixBuffer, 0);

	//정점 셰이더에서 상수 버퍼의 위치 설정
	bufferNumber = 0;

	//마지막으로 갱싱된 값으로 정점 셰이더의 상수 버퍼를 설정
	deviceContext->VSSetConstantBuffers(bufferNumber,1,&m_matrixBuffer);



	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	//이 삼각형을 렌더링하는 데 사용할 셰이더 설정함.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//렌더링
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
