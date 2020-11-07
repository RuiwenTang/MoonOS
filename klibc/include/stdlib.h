/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Tuesday, 3rd November 2020 12:54:31 pm                        *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#ifndef __MOON_OS_STDLIB_H__
#define __MOON_OS_STDLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

int katoi(const char* str);

char* kulltoa(unsigned long long value, char* str, int base);

char* klltoa(long long value, char* str, int base);

#ifdef __cplusplus
}
#endif

#endif  // __MOON_OS_STDLIB_H__