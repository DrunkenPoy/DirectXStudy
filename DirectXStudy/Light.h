#pragma once
#ifndef __LIGHT_H__
#define __LIGHT_H__

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
#include "myMacro.h"

using namespace DirectX;

class CLight
{
	CONSTRUCTION_FEILD_B(CLight,public);
public:
	void SetDiffuseColor(float r, float g, float b, float a) {m_diffuseColor = XMFLOAT4(r, g, b, a); };
	void SetDirection(float x, float y, float z) {m_direction = XMFLOAT3(x, y, z); };

	XMFLOAT4 GetDiffuseColor() { return m_diffuseColor; };
	XMFLOAT3 GetDirection() { return m_direction; };

private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
};

#endif __LIGHT_H__
