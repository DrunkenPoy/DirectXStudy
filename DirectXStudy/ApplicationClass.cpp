#include "ApplicationClass.h"

//생성자 정의
ApplicationClass::ApplicationClass()
{
	m_direct3D = nullptr;
}

//복사 생성자 정의
ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
	m_direct3D = nullptr;
}

//소멸자 정의
ApplicationClass::~ApplicationClass()
{
}	

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_direct3D = new D3DClass;
	NULL_CHECK_RETURN(m_direct3D, false);

	if (
		!m_direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR)
		)
	{
		MessageBox(hwnd, L"Direct3D 초기화 실패", L"Error", MB_OK);
		return false;
	}
	return true;
}

void ApplicationClass::Shutdown()
{
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
	if(!Render())
		return false;
	
	return true;
}

bool ApplicationClass::Render()
{
	//Direct3D로 장면을 렌더링한다.
	m_direct3D->beginScene(0.f, 1.f, 0.f, 1.f);

	m_direct3D->EndScene();

	return true;
}
