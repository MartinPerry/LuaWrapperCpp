#ifndef _MY_STRING_MACROS_H_
#define _MY_STRING_MACROS_H_


//#define CONSTEXPR_DISABLE 


#ifndef SWAP
	//http://www.cprogramming.com/tutorial/cpreprocessor.html
	#define SWAP(a, b)  do { a ^= b; b ^= a; a ^= b; } while ( 0 )
#endif

#ifndef SAFE_DELETE
	#define SAFE_DELETE(a) {if (a != NULL) { delete   a; a = NULL; }};
#endif

#ifndef SAFE_DELETE_ARRAY
	#define SAFE_DELETE_ARRAY(a) {if (a != NULL) { delete[] a; a = NULL; }};
#endif






#ifdef _MSC_VER
	#ifndef my_fopen 
		#define my_fopen(a, b, c) fopen_s(a, b, c)	
	#endif

	#ifndef my_localtime
		#define my_localtime(a, b) localtime_s(a, b)	
	#endif

	#ifndef FORCE_INLINE
		#define FORCE_INLINE	__forceinline
	#endif

#else

	#ifndef my_fopen 
		#define my_fopen(a, b, c) (*a = fopen(b, c))
	#endif

	#ifndef my_localtime
		#define my_localtime(a, b) (a = localtime(b))	
	#endif

	#ifndef FORCE_INLINE
		#define	FORCE_INLINE inline __attribute__((always_inline))
	#endif
#endif





typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;


//typedef unsigned int uint32;



#endif