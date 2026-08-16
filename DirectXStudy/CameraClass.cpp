#include "CameraClass.h"

CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;
	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
	m_viewMatrix = XMMatrixIdentity();
}

CameraClass::CameraClass(const CameraClass& other)
{
	m_positionX = other.m_positionX;
	m_positionY = other.m_positionY;
	m_positionZ = other.m_positionZ;
	m_rotationX = other.m_rotationX;
	m_rotationY = other.m_rotationY;
	m_rotationZ = other.m_rotationZ;
	m_viewMatrix = other.m_viewMatrix;
}

CameraClass::~CameraClass()
{
}

//함수 정의하기
void CameraClass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//업벡터
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//XMVECTOR 구조체에 설정한다.
	upVector = XMLoadFloat3(&up);

	//월드 공간에서 카메라의 위치를 설정한다.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
   
	positionVector = XMLoadFloat3(&position);
	

	//월드 공간에서 카메라의 위치를 설정한다.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	pitch = DegreeToRadian(m_rotationX);
	yaw = DegreeToRadian(m_rotationY);
	roll = DegreeToRadian(m_rotationZ);

	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	lookAtVector = XMVector3TransformCoord(lookAtVector,rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

}