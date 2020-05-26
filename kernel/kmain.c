#include <moonos/arch/tty.h>
#include <moonos/interupt.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <moonos/memory/balloc.h>
#include <moonos/time.h>
#include <multiboot/multiboot.h>

void main(uintptr_t mb_info_phys) {
    terminal_initialize();

    const struct multiboot_info* info =
            (const struct multiboot_info*)va(mb_info_phys);

    ints_setup();
    balloc_setup(info);
    time_setup();
    local_int_enable();

    while (1)
        ;
}