#include "ApplicationClass.h"
#include "CameraClass.h"

//생성자 정의
ApplicationClass::ApplicationClass()
{
	m_direct3D = nullptr;
	m_camera = nullptr;
	m_model = nullptr;
	//m_colorShader = nullptr;
	//m_textureShader = nullptr;
	m_light = nullptr;
	m_lightShader = nullptr;
	m_rotation = 0.f;
}

//복사 생성자 정의
ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
	m_direct3D = nullptr;
	m_camera = nullptr;
	m_model = nullptr;
	//m_colorShader = nullptr;
	//m_textureShader = nullptr;
	m_light = nullptr;
	m_lightShader = nullptr;

	m_rotation = 0.f;
}

//소멸자 정의
ApplicationClass::~ApplicationClass()
{
}	

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	char modelFilename[128];
	char textureFilename[128];


	m_direct3D = new D3DClass;
	NULL_CHECK_RETURN(m_direct3D, false);

	if (
		!m_direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)
		)
	{
		MessageBox(hwnd, L"Direct3D 초기화 실패", L"Error", MB_OK);
		return false;
	}

	ID3D11Device* pDevice = m_direct3D->GetDevice();
	NULL_CHECK_RETURN(pDevice, false);

	//카메라와 셰이더 객체들을 생성
	m_camera = new CameraClass;
	NULL_CHECK_RETURN(m_camera, false);

	m_camera->SetPosition(0.0f, 0.0f, -5.0f);

	strcpy_s(textureFilename, "../data/stone01.tga");
	strcpy_s(modelFilename, "../data/cube.txt");

	m_model = new ModelClass;
	if (!m_model->Initialize(pDevice, m_direct3D->GetDeviceContext(), textureFilename, modelFilename))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	/*m_colorShader = new ColorShaderClass;
	if(!m_colorShader->Initialize(pDevice,hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}*/
	/*m_textureShader = new CTextureShader;
	if (!m_textureShader->Initialize(pDevice, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}*/

	m_lightShader = new CLightShader;
	if (!m_lightShader->Initialize(pDevice, hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}


	
	m_light = new CLight;

	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void ApplicationClass::Shutdown()
{
	
	ReleasePtr(m_light);

	if (m_lightShader)
	{
		m_lightShader->Shutdown();
		ReleasePtr(m_lightShader);
	}
	//if (m_colorShader)
	//{
	//	m_colorShader->Shutdown();
	//	ReleasePtr(m_colorShader);
	//}
	//if (m_textureShader)
	//{
	//	m_textureShader->Shutdown();
	//	ReleasePtr(m_textureShader);
	//}
	if (m_model)
	{
		m_model->Shutdown();
		ReleasePtr(m_model);
	}
	ReleasePtr(m_camera);

	//Direct3D 객체를 종료하고 메모리 해제
	if (m_direct3D)
	{
		m_direct3D->Shutdown();
		ReleasePtr(m_direct3D);
	}

	return;
}

bool ApplicationClass::Frame()
{
	
	m_rotation -= 0.0174532925f * 0.25f;

	if(m_rotation < 0.0f)
		m_rotation += 360.0f;

	if(!Render(m_rotation))
		return false;
	
	return true;
}

bool ApplicationClass::Render(float rotation)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	ID3D11DeviceContext* pDeviceContext = nullptr;
	pDeviceContext = m_direct3D->GetDeviceContext();
	NULL_CHECK_RETURN(pDeviceContext, false);


	//Direct3D로 장면을 렌더링한다.
	m_direct3D->beginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_camera->Render();

	m_direct3D->GetWorldMatrix(worldMatrix);
	m_camera->GetViewMatrix(viewMatrix);
	m_direct3D->GetPrjMatrix(projectionMatrix);

	worldMatrix = XMMatrixRotationY(rotation)* XMMatrixRotationX(rotation);

	m_model->Render(pDeviceContext);

	/*if (!m_colorShader->Render(pDeviceContext, m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
		return false;*/

	/*if (!m_textureShader->Render(pDeviceContext, m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,m_model->GetTexture()))
		return false;*/
	if (!m_lightShader->Render(pDeviceContext, m_model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_model->GetTexture(), m_light->GetDirection(), m_light->GetDiffuseColor()))
		return false;

	m_direct3D->EndScene();

	return true;
}
