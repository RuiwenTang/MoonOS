#include <string.h>

char* strcat(char* s1, const char* s2) {
    char* rc = s1;
    if (*s1) {
        while (*++s1)
            ;
    }
    while ((*s1++ = *s2++))
        ;

    return rc;
}