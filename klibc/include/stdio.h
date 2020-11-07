/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Tuesday, 3rd November 2020 12:54:01 pm                        *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#ifndef __MOON_OS_STDIO_H__
#define __MOON_OS_STDIO_H__
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int ksnprintf(char* str, size_t size, const char* format, ...);
int ksprintf(char* str, const char* format, ...);
int kvsnprintf(char* str, size_t size, const char* format, va_list ap);
int kvsprintf(char* str, const char* format, va_list ap);


#ifdef __cplusplus
}
#endif

#endif  // __MOON_OS_STDIO_H__