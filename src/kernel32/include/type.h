
#ifndef __TYPE__
#define __TYPE__

#ifdef __cplusplus
#define EXTERN_C extern "C" {
#define EXTERN_C_END }
#else
#define EXTERN_C
#define EXTERN_C_END
#endif

EXTERN_C

#include    <stdint.h>
//#define int8_t signed char
//#define uint8_t unsigned char
//#define int16_t signed short int
//#define uint16_t unsigned short int
//#define int32_t signed long int
//#define uint32_t unsigned long int
//#define int64_t signed long long int
//#define uint64_t unsigned long long int

#ifndef size_t
#define size_t uint32_t
#endif

//Note: 'uintptr_t' is the size of the default memory reference. If a 32-bit Protected Mode setup, it's equivalent to uint32_t, and uint64_t in 64-bit Long Mode environments.
#ifndef uintptr_t
#define uintptr_t uint32_t
#endif

#define greg_t  uintptr_t       //general register type

#define byte unsigned char
#define word uint16_t
#define dword uint32_t

#define NULL    0
#define NULLPTR 0

#ifdef __cplusplus
// C++ code

#else
// C code
    #define bool _Bool
    #define true 1
    #define false 0
#endif


typedef int (*interrupt_handler)(void*, uint32_t);  //(void* esp, uint32_t int_id)
//return 1 for failure //return 0 to iretd
//if nullptr is specified, a 1 will be returned


EXTERN_C_END
#endif
