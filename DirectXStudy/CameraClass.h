#pragma once
#ifndef __CAMERACLASS_H__
#define __CAMERACLASS_H__

//////////////
// INCLUDES //
//////////////
#include <directxmath.h>
#include "myMacro.h"

using namespace DirectX;


class CameraClass
{
	CONSTRUCTION_FEILD(CameraClass);
public:
	void SetPosition(float x, float y, float z) 
	{ m_positionX = x; m_positionY = y; m_positionZ = z; return;};
	void SetRotation(float x, float y, float z)
	{ m_rotationX = x; m_rotationY = y; m_rotationZ = z; return; };

	XMFLOAT3 GetPosition() { return XMFLOAT3(m_positionX, m_positionY, m_positionZ); };
	XMFLOAT3 GetRotation() { return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ); };

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; return; };

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif __CAMERACLASS_H__