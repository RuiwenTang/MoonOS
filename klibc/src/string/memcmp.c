#include <string.h>

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = s1;
    const unsigned char* p2 = s2;

    while (n--) {
        const int r = *p1++ - *p2++;
        if (r) {
            return r;
        }
    }

    return 0;
}