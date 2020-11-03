#include <stdio.h>

int ksprintf(char* const str, const char* const format, ...) {
    va_list ap;
    va_start(ap, format);
    const int result = kvsprintf(str, format, ap);
    va_end(ap);
    return result;
}
