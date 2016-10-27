#ifndef MACROS_H
#define MACROS_H

//Defines


//Includes

#include "./Logger.h"

//Macros

#ifndef SWAP
	//http://www.cprogramming.com/tutorial/cpreprocessor.html
	#define SWAP(a, b)  do { a ^= b; b ^= a; a ^= b; } while ( 0 )
#endif

#define SAFE_DELETE(a) {if (a != NULL) { delete   a; a = NULL; }};
#define SAFE_DELETE_ARRAY(a) {if (a != NULL) { delete[] a; a = NULL; }};


#define BIT(x) (1<<(x))


#ifdef _MSC_VER
	#ifndef my_fopen 
		#define my_fopen(a, b, c) fopen_s(a, b, c)	
	#endif
	#ifndef my_localtime
		#define my_localtime(a, b) localtime_s(a, b)	
	#endif
#else
	#ifndef my_fopen 
		#define my_fopen(a, b, c) (*a = fopen(b, c))
	#endif
	#ifndef my_localtime
		#define my_localtime(a, b) (a = localtime(b))	
	#endif
#endif

#ifndef MY_LOG_ERROR
	#define MY_LOG_ERROR(...) MyUtils::Logger::LogError(__VA_ARGS__)
#endif

#ifndef MY_LOG_WARNING
	#define MY_LOG_WARNING(...) MyUtils::Logger::LogWarning(__VA_ARGS__)
#endif

#ifndef MY_LOG_INFO
	#define MY_LOG_INFO(...) MyUtils::Logger::LogInfo(__VA_ARGS__)
#endif

#ifndef MY_LOG
	#define MY_LOG(...) MyUtils::Logger::LogMessage(__VA_ARGS__)
#endif

#ifndef MARK_TO_DO
	#define MARK_TO_DO MyUtils::Logger::LogMessage("TO DO method - line %d in %s\n", __LINE__, __FILE__)
#endif

#ifndef MARK_DEPRECATED
	#define MARK_DEPRECATED MyUtils::Logger::LogMessage("Deprecated method / object - line %d in %s\n", __LINE__, __FILE__)
#endif


#ifndef GET_DYNAMIC_CAST
	#define GET_DYNAMIC_CAST(Type, Value) \
		virtual Type * Get##Type() { \
           return Value; \
		};
#endif

#ifndef GET_DYNAMIC_CAST_CONST
#define GET_DYNAMIC_CAST_CONST(Type, Value) \
		virtual const Type * Get##Type() const { \
           return Value; \
		};
#endif


//typedefs


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;


typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef const char * LPCSTR;
typedef const wchar_t * LPCWSTR;

//typedef unsigned int uint32;


#endif