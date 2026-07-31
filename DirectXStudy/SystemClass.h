#pragma once
#ifndef __SYSTEMCLASS_H__
#define __SYSTEMCLASS_H__

#include <windows.h>

#include "inputclass.h"
#include "applicationclass.h"
#include "myMacro.h"

class SystemClass
{
	CONSTRUCTION_FEILD(SystemClass)

public:
	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK  MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int& ,int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputClass* m_Input;
	ApplicationClass* m_Application;

};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* g_ApplicationHandle = nullptr;


#endif 

