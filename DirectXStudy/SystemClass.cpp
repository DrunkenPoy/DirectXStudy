#include "SystemClass.h"

SystemClass::SystemClass()
{
	m_Input = nullptr;
	m_Application = nullptr;
}

SystemClass::SystemClass(const SystemClass& param)
{
	m_Input = nullptr;
	m_Application = nullptr;
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;
	bool result;

	InitializeWindows(screenWidth, screenHeight);


	// 입력 객체를 생성하고 초기화한다. 이 객체는 사용자의 키보드 입력을 읽어 처리하는 데 사용된다.
	m_Input = new InputClass;
	m_Input->Initialize();

	// 애플리케이션 클래스 객체를 생성하고 초기화한다. 이 객체는 이 애플리케이션의 모든 그래픽 렌더링을 담당한다.
	m_Application = new ApplicationClass;
	result = m_Application->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	if (m_Application)
	{
		m_Application->Shutdown();	
	}

	ReleasePtr(m_Application);
	ReleasePtr(m_Input);

	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;
	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			done = true;
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam , LPARAM lparam)
{

	switch (umsg)
	{
	case WM_KEYDOWN:
	{
		// 키가 눌리면 입력 객체로 보내 그 상태를 기록하게 한다.
		m_Input->KeyDown((uint)wparam);
		return 0;
	}
	case WM_KEYUP:
	{
		// 키가 떼어지면 입력 객체로 보내 그 키의 상태를 해제하게 한다.
		m_Input->KeyUp((uint)wparam);
		return 0;
	}
	default:
		return DefWindowProc(hwnd, umsg, wparam, lparam);
	}

	return LRESULT();

}

bool SystemClass::Frame()
{
	bool result;

	// 사용자가 ESC를 눌러 애플리케이션을 종료하려는지 확인한다.
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// 애플리케이션 클래스 객체의 프레임 처리를 수행한다.
	result = m_Application->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	g_ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	m_applicationName = L"DirectXStudy_DKP";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH); // 여기서 배경 검은색 처리
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (ulong)screenWidth;
		dmScreenSettings.dmPelsHeight = (ulong)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		// 창을 화면 중앙에 배치한다.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// 창을 화면에 띄우고 메인 포커스로 설정한다.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 마우스 커서를 숨긴다.
	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	g_ApplicationHandle = NULL;

	return;

}

LRESULT WndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		return g_ApplicationHandle->MessageHandler(hwnd, umsg, wparam, lparam);
	}
}