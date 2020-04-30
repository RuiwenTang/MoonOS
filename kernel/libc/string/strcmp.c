#include <string.h>

int strcmp(const char* const s1, const char* const s2) {
    const uint8_t* p1 = (uint8_t*)s1;
    const uint8_t* p2 = (uint8_t*)s2;

    while (*p1 != '\0' && *p1 == *p2) {
        p1++;
        p2++;
    }

    return *p1 - *p2;
}