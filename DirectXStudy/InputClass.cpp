#include "InputClass.h"

InputClass::InputClass()
{
	for (int i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}
}


InputClass::InputClass(const InputClass& other)
{
	for (int i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	for (int i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(uint input)
{
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(uint input)
{
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(uint key)
{
	return m_keys[key];
}
