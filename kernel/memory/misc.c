#include <moonos/kprintf.h>
#include <moonos/memory.h>
#include <moonos/memory/misc.h>
#include <multiboot/multiboot.h>
#include <stddef.h>
#include <string.h>

uintptr_t initrd_begin;
uintptr_t initrd_end;

uintptr_t mmap_begin;
uintptr_t mmap_end;

static void mmap_find(const struct multiboot_info* info) {
    if ((info->flags & MULTIBOOT_INFO_MEM_MAP) == 0) {
        kprintf("No memory map provided\n");
        while (1)
            ;
    }

    mmap_begin = info->mmap_addr;
    mmap_end = mmap_begin + info->mmap_length;
}

static void initrd_find(const multiboot_info_t* info) {
    if (!(info->flags & MULTIBOOT_INFO_MODS)) {
        kprintf("No initramfs provided\n");
        while (1)
            ;
    }

    const multiboot_module_t* mods = va(info->mods_addr);
    const size_t count = info->mods_count;
    kprintf("mod count = %d \n", count);
    for (size_t i = 0; i != count; i++) {
        static const char sign[] = "070701";
        const multiboot_module_t* mod = &mods[i];
        if (mod->mod_end - mod->mod_start < sizeof(sign) - (uint32_t)1) {
            continue;
        }

        kprintf("size check pass \n");

        const void* ptr = va(mod->mod_start);
        kprintf("mod_start = %x\n", mod->mod_start);
        int ret = memcmp(ptr, sign, sizeof(sign) - 1);
        if (ret) {
            kprintf("sign not match ret = %d\n", ret);
            continue;
        }

        initrd_begin = mod->mod_start;
        initrd_end = mod->mod_end;
        return;
    }
    kprintf("No initramfs 123123 provided\n");
    while (1)
        ;
}

void misc_setup(const multiboot_info_t* info) {
    mmap_find(info);
    initrd_find(info);
}