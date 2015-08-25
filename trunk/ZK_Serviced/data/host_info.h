#ifndef __host_info_h__
#define __host_info_h__


#include <iostream>

#ifdef __cplusplus
	extern "C"
	{
#endif

std::string getInfo(const std::string& type);

#ifdef __cplusplus
	}
#endif

#endif // host_info.h
