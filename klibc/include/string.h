/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Tuesday, 3rd November 2020 12:54:50 pm                        *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#ifndef __MOON_OS_STRING_H__
#define __MOON_OS_STRING_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memchr(const void* s, int c, size_t n);

int memcmp(const void* s1, const void* s2, size_t n);

void* memcpy(void* s1, const void* s2, size_t n);

void* memmove(void* s1, const void* s2, size_t n);

void* memset(void* s, int c, size_t n);

char* strcat(char* s1, const char* s2);

char* strchr(const char* s, int c);

int strcmp(const char* s1, const char* s2);

int strcoll(const char* s1, const char* s2);

char* strcpy(char* s1, const char* s2);

size_t strspn(const char* s1, const char* s2);

char* strerror(int errnum);

size_t strlen(const char* s);

#ifdef __cplusplus
}
#endif

#endif  // __MOON_OS_STRING_H__