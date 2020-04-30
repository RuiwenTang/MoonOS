#ifndef __MOONOS_STDIO_H__
#define __MOONOS_STDIO_H__
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int snprintf(char* str, size_t size, const char* format, ...);
int sprintf(char* str, const char* format, ...);
int vsnprintf(char* str, size_t size, const char* format, va_list ap);
int vsprintf(char* str, const char* format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif  // __MOONOS_STDIO_H__