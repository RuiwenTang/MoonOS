#include <stdio.h>

int ksnprintf(char* const str, const size_t size, const char* const format,
              ...) {
    va_list ap;
    va_start(ap, format);
    const int result = kvsnprintf(str, size, format, ap);
    va_end(ap);
    return result;
}