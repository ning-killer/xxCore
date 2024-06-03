#ifndef __TS_RNE_TYPE_H__
#define __TS_RNE_TYPE_H__

#ifdef __KERNEL__

#include <linux/types.h>
#else

#include <stdbool.h>
#include <stdint.h>
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*--------------------------------------------------------------------------------------------------------------*
 * Defintion of basic data types. The data types are applicable to both the application layer and kernel codes. *
 *--------------------------------------------------------------------------------------------------------------*/
/*************************** Structure Definition ****************************/
/** \addtogroup      Common_TYPE */
/** @{ */ /** <!-- [Common_TYPE] */

typedef unsigned char TS_UCHAR;
typedef unsigned char TS_U8;
typedef unsigned short TS_U16;
typedef unsigned int TS_U32;
typedef unsigned long TS_UL;
typedef TS_UL TS_ULONG;
typedef uintptr_t TS_UINTPTR_T;

typedef char TS_CHAR;
typedef signed char TS_S8;
typedef short TS_S16;
typedef int TS_S32;
typedef long TS_SL;

typedef float TS_FLOAT;
typedef double TS_DOUBLE;

#ifndef _M_IX86
typedef unsigned long long TS_U64;
typedef long long TS_S64;
#else
typedef unsigned __int64 TS_U64;
typedef __int64 TS_S64;
#endif

typedef unsigned long TS_SIZE_T;
typedef unsigned long TS_LENGTH_T;
typedef unsigned long int TS_PHYS_ADDR_T;

typedef unsigned int TS_HANDLE;
typedef bool TS_BOOL;

/*----------------------------------------------*
 * const defination                             *
 *----------------------------------------------*/

#ifndef TS_FALSE
#define TS_FALSE 0
#endif

#ifndef TS_TRUE
#define TS_TRUE 1
#endif

#ifndef NULL
#define NULL 0L
#endif

#ifndef TS_NULL
#define TS_NULL 0L
#endif

#ifndef TS_SUCCESS
#define TS_SUCCESS 0
#endif

#ifndef TS_FAILURE
#define TS_FAILURE (-1)
#endif

#ifndef TS_VOID
#define TS_VOID void
#endif

typedef unsigned char TS_uchar;
typedef unsigned char TS_u8;
typedef unsigned short TS_u16;
typedef unsigned int TS_u32;
typedef unsigned long TS_ulong;

typedef char TS_char;
typedef signed char TS_s8;
typedef short TS_s16;
typedef int TS_s32;
typedef long TS_slong;

typedef float TS_float;
typedef double TS_double;

typedef void TS_void;

#ifndef _M_IX86
typedef unsigned long long TS_u64;
typedef long long TS_s64;
#else
typedef unsigned __int64 TS_u64;
typedef __int64 TS_s64;
#endif

typedef unsigned long TS_size_t;
typedef unsigned long TS_length_t;

typedef TS_u32 TS_handle;

typedef TS_BOOL TS_bool;
typedef TS_UINTPTR_T TS_uintptr_t;
typedef unsigned long int TS_phys_addr_t;

/** @} */ /** <!-- ==== Structure Definition end ==== */

#define IN
#define OUT

#define TS_UNUSED(x) ((x) = (x))

#define DEBUG_PRT(fmt...)                            \
    do {                                             \
        printf("[%s]-%d: ", __FUNCTION__, __LINE__); \
        printf(fmt);                                 \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_RNE_TYPE_H__ */
