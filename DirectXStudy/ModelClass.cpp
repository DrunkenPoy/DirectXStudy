#include "modelClass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_texture = nullptr;

	m_vertexCount = 3;
	m_indexCount = 3;
}
ModelClass::ModelClass(const ModelClass& other)
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_texture = nullptr;

	m_vertexCount = 3;
	m_indexCount = 3;
}
ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* pDevice)
{
	NULL_CHECK_RETURN(pDevice, false);

	if (!InitializeBuffer(pDevice))
		return false;
	return true;
}

bool ModelClass::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	NULL_CHECK_RETURN(pDevice, false);

	if (!InitializeBuffer(pDevice))
		return false;

	if (!LoadTexture(pDevice, deviceContext, textureFilename))
		return false;

	return true;
}
void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffer();
	return;
}

void ModelClass::Render(ID3D11DeviceContext* pDeviceContext)
{
	RenderBuffers(pDeviceContext);
	return;
}

bool ModelClass::InitializeBuffer(ID3D11Device* pDevice)
{
	VertexType* vertices;
	ULONG* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertices = new VertexType[m_vertexCount];
	NULL_CHECK_RETURN(vertices, false);
	
	indices = new ULONG[m_indexCount];
	NULL_CHECK_RETURN(indices, false);

	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); //좌하단.
	//vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	vertices[0].texcoordUV0 = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);	//상단 중앙.
	//vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vertices[1].texcoordUV0 = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); //우하단.
	//vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	vertices[2].texcoordUV0 = XMFLOAT2(1.0f, 1.0f);


	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	//정적 정점 버퍼의 구조체를 설정
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//서브 리소스 구조체에 정점 데이터의 포인터를 지정한다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	//버텍스 버퍼를 생성		
	FAILED_CHECK_RETURN(pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer), false);

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(ULONG) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//인덱스 버퍼를 생성		
	FAILED_CHECK_RETURN(pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer), false);

	MacroFunctor::FReleaseArr fReleaseArr;
	fReleaseArr(vertices);
	fReleaseArr(indices);

	return true;
}

void ModelClass::ShutdownBuffer()
{
	ReleaseCOM_Ptr(m_indexBuffer);
	ReleaseCOM_Ptr(m_vertexBuffer);
	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	//정점 버퍼의 스트라이드와 오프셋을 설정
	uint stride = sizeof(VertexType);
	uint offset = 0;

	//렌더링할 수 있도록 입력 어셈블러에서 정점 버퍼 활성화
	deviceContext->IASetVertexBuffers(0,1,&m_vertexBuffer, &stride, &offset);

	//렌더링할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성화
	deviceContext->IASetIndexBuffer(m_indexBuffer,DXGI_FORMAT_R32_UINT, offset);

	//정점 버퍼로 그릴 프리미티브 종류를 설정(삼각형)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* pDevice, ID3D11DeviceContext* deviceContext, char* filename)
{
	m_texture = new CTextureResource;

	if (!m_texture->Initialize(pDevice, deviceContext, filename))
		return false;

	return true;
}

void ModelClass::ReleaseTexture()
{
	if (m_texture)
	{
		m_texture->Shutdown();
		ReleasePtr(m_texture);
	}
}
