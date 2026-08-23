#pragma once
#ifndef __MYMACRO_H__
#define __MYMACRO_H__

#include "DirectXMath.h"
using namespace DirectX;

typedef struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
}MatrixBuffer;

#define ReleasePtr(ptr)		\
if(ptr)						\
{							\
	delete ptr;				\
	ptr = nullptr;			\
}							\


#define ReleaseArr(arr)		\
if(arr)						\
{							\
	delete[] arr;			\
	arr = nullptr;			\
}							\

#define ReleaseCOM_Ptr(ptr)		\
if(ptr)						\
{							\
	ptr->Release();			\
	ptr = nullptr;			\
}							\

			
#define CONSTRUCTION_FEILD(ClassName)	\
public:									\
	ClassName();						\
	ClassName(const ClassName&);		\
	~ClassName();						\

#define CONSTRUCTION_FEILD_B(ClassName,Access)	\
Access:											\
	ClassName();								\
	ClassName(const ClassName&);				\
	~ClassName();								\

#define IMPLEMENT_CONSTRUCTION_FEILD(ClassName)		\
ClassName::ClassName()								\
{													\
}													\
ClassName::ClassName(const ClassName& other)		\
{													\
}													\
ClassName::~ClassName()								\
{													\
}													\

#define NULL_CHECK_RETURN(_ptr,	_return)	\
{if(!_ptr){__debugbreak();return _return;}}

#define FAILED_CHECK_RETURN(_hresult,_return)	\
{if(FAILED((HRESULT)_hresult)){__debugbreak();return _return;}}

static float const g_PI = 3.141592654f;
static float const g_DegToRad = g_PI / 180.0f;

#define PI 3.141592654f
#define DEG2RAD 0.0174532925.0f
#define DegreeToRadian(Degree) Degree * 0.0174532925f				


typedef unsigned int uint;
typedef unsigned long ulong;


#endif __MYMACRO_H__