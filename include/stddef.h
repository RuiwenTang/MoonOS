#ifndef __MOON_OS_STDDEF_H__
#define __MOON_OS_STDDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long size_t;

typedef long ptrdiff_t;
typedef long off_t;

typedef long long ssize_t;
typedef long long soff_t;

typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;

#ifndef NULL
#define NULL 0
#endif

#ifdef __cplusplus
}
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#ifndef _GXX_NULLPTR_T
#define _GXX_NULLPTR_T
typedef decltype(nullptr) nullptr_t;
#endif
#endif /* C++11.  */

#endif  // __MOON_OS_STDDEF_H__