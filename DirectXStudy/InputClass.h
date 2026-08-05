#pragma once
#ifndef __INPUTCLASS_H__
#define __INPUTCLASS_H__

#include "myMacro.h"

class InputClass
{
	CONSTRUCTION_FEILD(InputClass)
public:
	void Initialize();

	void KeyDown(uint);
	void KeyUp(uint);

	bool IsKeyDown(uint);
private:
	bool m_keys[256];
};

#endif __INPUTCLASS_H__