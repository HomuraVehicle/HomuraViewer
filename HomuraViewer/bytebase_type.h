#ifndef HMR_BYTEBASETYPE_INC
#define HMR_BYTEBASETYPE_INC 102
#
#include<cstdint>

#ifndef HMLIB_CONFIG_INC
#	include "hmLib_config.h"
#endif
#//StdTypeSize
#if HMLIB_TYPE_SIZE_CHAR==1
#	ifndef HMLIB_TYPE_SIZE_8
#		define HMLIB_TYPE_SIZE_8
		typedef unsigned char std::uint8_t;
		typedef signed char hmLib_sint8;
#	endif
#elif HMLIB_TYPE_SIZE_CHAR==2
#	ifndef HMLIB_TYPE_SIZE_16
#		define HMLIB_TYPE_SIZE_16
		typedef unsigned char std::uint16_t;
		typedef signed char hmLib_sint16;
#	endif
#endif
#if HMLIB_TYPE_SIZE_SHORT==1
#	ifndef HMLIB_TYPE_SIZE_8
#		define HMLIB_TYPE_SIZE_8
		typedef unsigned short std::uint8_t;
		typedef signed short hmLib_sint8;
#	endif
#elif HMLIB_TYPE_SIZE_SHORT==2
#	ifndef HMLIB_TYPE_SIZE_16
#		define HMLIB_TYPE_SIZE_16
		typedef unsigned short std::uint16_t;
		typedef signed short hmLib_sint16;
#	endif
#elif HMLIB_TYPE_SIZE_SHORT==4
#	ifndef HMLIB_TYPE_SIZE_32
#		define HMLIB_TYPE_SIZE_32
		typedef unsigned short hmLib_uint32;
		typedef signed short hmLib_sint32;
#	endif
#endif
#if HMLIB_TYPE_SIZE_INT==1
#	ifndef HMLIB_TYPE_SIZE_8
#		define HMLIB_TYPE_SIZE_8
		typedef unsigned int std::uint8_t;
		typedef signed int hmLib_sint8;
#	endif
#elif HMLIB_TYPE_SIZE_INT==2
#	ifndef HMLIB_TYPE_SIZE_16
#		define HMLIB_TYPE_SIZE_16
		typedef unsigned int std::uint16_t;
		typedef signed int hmLib_sint16;
#	endif
#elif HMLIB_TYPE_SIZE_INT==4
#	ifndef HMLIB_TYPE_SIZE_32
#		define HMLIB_TYPE_SIZE_32
		typedef unsigned int hmLib_uint32;
		typedef signed int hmLib_sint32;
#	endif
#elif HMLIB_TYPE_SIZE_INT==8
#	ifndef HMLIB_TYPE_SIZE_64
#		define HMLIB_TYPE_SIZE_64
		typedef unsigned int hmLib_uint64;
		typedef signed int hmLib_sint64;
#	endif
#endif
#if HMLIB_TYPE_SIZE_LONG==1
#	ifndef HMLIB_TYPE_SIZE_8
#		define HMLIB_TYPE_SIZE_8
		typedef unsigned long std::uint8_t;
		typedef signed long hmLib_sint8;
#	endif
#elif HMLIB_TYPE_SIZE_LONG==2
#	ifndef HMLIB_TYPE_SIZE_16
#		define HMLIB_TYPE_SIZE_16
		typedef unsigned long std::uint16_t;
		typedef signed long hmLib_sint16;
#	endif
#elif HMLIB_TYPE_SIZE_LONG==4
#	ifndef HMLIB_TYPE_SIZE_32
#		define HMLIB_TYPE_SIZE_32
		typedef unsigned long hmLib_uint32;
		typedef signed long hmLib_sint32;
#	endif
#elif HMLIB_TYPE_SIZE_LONG==8
#	ifndef HMLIB_TYPE_SIZE_64
#		define HMLIB_TYPE_SIZE_64
		typedef unsigned long hmLib_uint64;
		typedef signed long hmLib_sint64;
#	endif
#elif HMLIB_TYPE_SIZE_LONG==16
#	ifndef HMLIB_TYPE_SIZE_128
#		define HMLIB_TYPE_SIZE_128
		typedef unsigned long hmLib_uint128;
		typedef signed long hmLib_sint128;
#	endif
#endif
#if HMLIB_TYPE_SIZE_LONG_LONG==1
#	ifndef HMLIB_TYPE_SIZE_8
#		define HMLIB_TYPE_SIZE_8
		typedef unsigned long long std::uint8_t;
		typedef signed long long hmLib_sint8;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_LONG==2
#	ifndef HMLIB_TYPE_SIZE_16
#		define HMLIB_TYPE_SIZE_16
		typedef unsigned long long std::uint16_t;
		typedef signed long long hmLib_sint16;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_LONG==4
#	ifndef HMLIB_TYPE_SIZE_32
#		define HMLIB_TYPE_SIZE_32
		typedef unsigned long long hmLib_uint32;
		typedef signed long long hmLib_sint32;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_LONG==8
#	ifndef HMLIB_TYPE_SIZE_64
#		define HMLIB_TYPE_SIZE_64
		typedef unsigned long long hmLib_uint64;
		typedef signed long long hmLib_sint64;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_LONG==16
#	ifndef HMLIB_TYPE_SIZE_128
#		define HMLIB_TYPE_SIZE_128
		typedef unsigned long long hmLib_uint128;
		typedef signed long long hmLib_sint128;
#	endif
#endif
#if HMLIB_TYPE_SIZE_FLOAT==1
#	ifndef HMLIB_TYPE_SIZE_f8
#		define HMLIB_TYPE_SIZE_f8
		typedef float hmLib_float8;
#	endif
#elif HMLIB_TYPE_SIZE_FLOAT==2
#	ifndef HMLIB_TYPE_SIZE_f16
#		define HMLIB_TYPE_SIZE_f16
		typedef float hmLib_float16;
#	endif
#elif HMLIB_TYPE_SIZE_FLOAT==4
#	ifndef HMLIB_TYPE_SIZE_f32
#		define HMLIB_TYPE_SIZE_f32
		typedef float hmLib_float32;
#	endif
#elif HMLIB_TYPE_SIZE_FLOAT==8
#	ifndef HMLIB_TYPE_SIZE_f64
#		define HMLIB_TYPE_SIZE_f64
		typedef float hmLib_float64;
#	endif
#elif HMLIB_TYPE_SIZE_FLOAT==16
#	ifndef HMLIB_TYPE_SIZE_f128
#		define HMLIB_TYPE_SIZE_f128
		typedef float hmLib_f128;
#	endif
#endif
#if HMLIB_TYPE_SIZE_DOUBLE==1
#	ifndef HMLIB_TYPE_SIZE_f8
#		define HMLIB_TYPE_SIZE_f8
		typedef double hmLib_float8;
#	endif
#elif HMLIB_TYPE_SIZE_DOUBLE==2
#	ifndef HMLIB_TYPE_SIZE_f16
#		define HMLIB_TYPE_SIZE_f16
		typedef double hmLib_float16;
#	endif
#elif HMLIB_TYPE_SIZE_DOUBLE==4
#	ifndef HMLIB_TYPE_SIZE_f32
#		define HMLIB_TYPE_SIZE_f32
		typedef double hmLib_float32;
#	endif
#elif HMLIB_TYPE_SIZE_DOUBLE==8
#	ifndef HMLIB_TYPE_SIZE_f64
#		define HMLIB_TYPE_SIZE_f64
		typedef double hmLib_float64;
#	endif
#elif HMLIB_TYPE_SIZE_DOUBLE==16
#	ifndef HMLIB_TYPE_SIZE_f128
#		define HMLIB_TYPE_SIZE_f128
		typedef double hmLib_f128;
#	endif
#endif
#if HMLIB_TYPE_SIZE_LONG_DOUBLE==1
#	ifndef HMLIB_TYPE_SIZE_f8
#		define HMLIB_TYPE_SIZE_f8
		typedef long double hmLib_float8;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_DOUBLE==2
#	ifndef HMLIB_TYPE_SIZE_f16
#		define HMLIB_TYPE_SIZE_f16
		typedef long double hmLib_float16;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_DOUBLE==4
#	ifndef HMLIB_TYPE_SIZE_f32
#		define HMLIB_TYPE_SIZE_f32
		typedef long double hmLib_float32;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_DOUBLE==8
#	ifndef HMLIB_TYPE_SIZE_f64
#		define HMLIB_TYPE_SIZE_f64
		typedef long double hmLib_float64;
#	endif
#elif HMLIB_TYPE_SIZE_LONG_DOUBLE==16
#	ifndef HMLIB_TYPE_SIZE_f128
#		define HMLIB_TYPE_SIZE_f128
		typedef long double hmLib_f128;
#	endif
#endif
//boolian
typedef unsigned char  hmLib_boolian;
#define hmLib_true 1
#define hmLib_false 0
//pi
#define hmLib_pi() (3.141592653589793)
//namespace_type
#ifdef __cplusplus
namespace hmLib{
	inline decltype(hmLib_pi()) pi(){return hmLib_pi();} 
	namespace type{
		//boolian
		typedef hmLib_boolian boolian;
		//hmStd Type
		#ifdef HMLIB_TYPE_SIZE_8
			typedef std::uint8_t uint8;
			typedef hmLib_sint8 sint8;
		#endif
		#ifdef HMLIB_TYPE_SIZE_16
			typedef std::uint16_t uint16;
			typedef hmLib_sint16 sint16;
		#endif
		#ifdef HMLIB_TYPE_SIZE_32
			typedef hmLib_uint32 uint32;
			typedef hmLib_sint32 sint32;
		#endif
		#ifdef HMLIB_TYPE_SIZE_64
			typedef hmLib_uint64 uint64;
			typedef hmLib_sint64 sint64;
		#endif
		#ifdef HMLIB_TYPE_SIZE_128
			typedef hmLib_uint128 uint128;
			typedef hmLib_sint128 sint128;
		#endif
		#ifdef HMLIB_TYPE_SIZE_f32
			typedef hmLib_float32 float32;
		#endif
		#ifdef HMLIB_TYPE_SIZE_f64
			typedef hmLib_float64 float64;
		#endif
		#ifdef HMLIB_TYPE_SIZE_f128
			typedef hmLib_f128 float128;
		#endif
		typedef signed char schar;
		typedef unsigned char uchar;
		typedef signed short sshort;
		typedef unsigned short ushort;
		typedef signed int sint;
		typedef unsigned int uint;
		typedef signed long slong;
		typedef unsigned long ulong;
		typedef signed long long sdlong;
		typedef unsigned long long udlong;
	}
}
#endif
#
#endif
