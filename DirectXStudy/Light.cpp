#include "Light.h"

CLight::CLight() 
{
	m_diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
}	

CLight::CLight(const CLight &other) 
{
	m_diffuseColor = other.m_diffuseColor;
	m_direction = other.m_direction;
}

CLight::~CLight() {}