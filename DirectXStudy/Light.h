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
	void SetSpecularColor(float r, float g, float b, float a) { m_specularColor = XMFLOAT4(r, g, b, a); };
	void SetSpecularPower(float power) { m_specularPower = power; };


	XMFLOAT4 GetDiffuseColor() { return m_diffuseColor; };
	XMFLOAT3 GetDirection() { return m_direction; };
	XMFLOAT4 GetSpecularColor() { return m_specularColor; };
	float GetSpecularPower() { return m_specularPower; };
private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_specularColor;
	float m_specularPower;
};

#endif __LIGHT_H__
