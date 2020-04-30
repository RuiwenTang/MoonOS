#include <stdbool.h>
#include <stdlib.h>
static bool isdigit(char c) { return c >= '0' && c <= '9'; }

int atoi(const char* str) {
    int value = 0;
    while (isdigit(*str)) {
        value *= 10;
        value += (*str) - '0';
        str++;
    }
    return value;
}