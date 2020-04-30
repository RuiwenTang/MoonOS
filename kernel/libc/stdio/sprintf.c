#include <stdio.h>

int sprintf(char* const str, const char* const format, ...) {
    va_list ap;
    va_start(ap, format);
    const int result = vsprintf(str, format, ap);
    va_end(ap);
    return result;
}