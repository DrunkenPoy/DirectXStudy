#include "ApplicationClass.h"

IMPLEMENT_CONSTRUCTION_FEILD(ApplicationClass)

bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	return true;
}

void ApplicationClass::Shutdown()
{
}

bool ApplicationClass::Frame()
{
	return true;
}

bool ApplicationClass::Render()
{
	return true;
}
