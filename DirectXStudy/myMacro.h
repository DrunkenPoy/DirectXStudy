#pragma once
#ifndef __MYMACRO_H__
#define __MYMACRO_H__


#define ReleasePtr(ptr)		\
if(ptr)						\
{							\
	delete ptr;				\
	ptr = nullptr;			\
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



typedef unsigned int uint;
typedef unsigned long ulong;


#endif