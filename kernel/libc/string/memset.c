#include <string.h>

void* memset(void* const s, const int c, size_t n) {
    const unsigned char b = (unsigned char)c;

    unsigned char* p = (unsigned char*)s;
    while (n--) {
        *p++ = b;
    }
    return s;
}