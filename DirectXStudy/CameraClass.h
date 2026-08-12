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
	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
};

#endif