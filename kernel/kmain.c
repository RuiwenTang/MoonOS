#include <moonos/arch/tty.h>
#include <multiboot/multiboot.h>
#include <stdio.h>
#include <stdlib.h>

void main() {
    terminal_initialize();
    char buffer[100] = {0};

    int tail = ksprintf(buffer, "Hello World memory = %d", 123);
    buffer[tail] = '\0';

    terminal_writestring(buffer);
}