#include <moonos/arch/tty.h>
#include <moonos/interupt.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <moonos/memory/balloc.h>
#include <moonos/memory/paging.h>
#include <moonos/time.h>
#include <multiboot/multiboot.h>
#include <string.h>

static void test_mapping(void) {
    const size_t count = balloc_free_ranges();
    for (size_t i = 0; i != count; i++) {
        struct balloc_range range;
        balloc_get_free_range(i, &range);
        kprintf("Fill 0x%llx - 0x%llx\n", (uint64_t)range.begin,
                (uint64_t)range.end);

        memset(va(range.begin), 42, range.end - range.begin);
    }
}

void main(uintptr_t mb_info_phys) {
    terminal_initialize();

    const struct multiboot_info* info =
            (const struct multiboot_info*)va(mb_info_phys);

    ints_setup();
    balloc_setup(info);
    paging_setup();
    time_setup();
    local_int_enable();

    test_mapping();

    // while (1)
    //     ;
}