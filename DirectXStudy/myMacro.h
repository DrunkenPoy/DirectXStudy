#pragma once
#ifndef __MYMACRO_H__
#define __MYMACRO_H__


#define ReleasePtr(ptr)		\
if(ptr)						\
{							\
	delete ptr;				\
	ptr = nullptr;			\
}							\
			


#endif