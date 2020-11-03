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

char* kulltoa(unsigned long long value, char* str, int base) {
    static const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* ptr = str;
    char* start = str;

    do {
        const int r = value % base;

        value = value / base;
        *ptr++ = digits[r];
    } while (value);

    *ptr-- = '\0';

    while (start < ptr) {
        const char c = *ptr;

        *ptr-- = *start;
        *start++ = c;
    }

    return str;
}

char* klltoa(long long value, char* str, int base) {
    char* ptr = str;
    if (value < 0) {
        *ptr++ = '-';
        value = -value;
    }

    kulltoa(value, ptr, base);

    return str;
}