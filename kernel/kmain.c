#include <moonos/arch/tty.h>
#include <moonos/interupt.h>
#include <moonos/kprintf.h>
#include <moonos/time.h>
#include <multiboot/multiboot.h>

void main() {
    terminal_initialize();

    kprintf("hello world %d \n 0x%llx ", 123, 0xffffffffffffu);

    ints_setup();
    time_setup();
    local_int_enable();

    while (1)
        ;
}