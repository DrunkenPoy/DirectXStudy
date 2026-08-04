#pragma once
#ifndef __MYMACRO_H__
#define __MYMACRO_H__


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


			
#define CONSTRUCTION_FEILD(ClassName)	\
public:									\
	ClassName();						\
	ClassName(const ClassName&);		\
	~ClassName();						\



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


typedef unsigned int uint;
typedef unsigned long ulong;


#endif