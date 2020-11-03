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