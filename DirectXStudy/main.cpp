#include "main.h"
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	std::cout<< L"HelloWorld" << std::endl;
	SystemClass* system;
	bool result;

	system = new SystemClass;
	result = system->Initialize();
	if (result)
	{
		system->Run();
	}

	system->Shutdown();
	ReleasePtr(system);

	return 0;
}
