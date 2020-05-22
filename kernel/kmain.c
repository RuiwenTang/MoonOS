#include <moonos/arch/tty.h>
#include <multiboot/multiboot.h>

void main() {
    terminal_initialize();
    terminal_putchar('c');
}