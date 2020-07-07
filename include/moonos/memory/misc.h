#ifndef __MISC_H__
#define __MISC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uintptr_t mmap_begin;
extern uintptr_t mmap_end;

extern uintptr_t initrd_begin;
extern uintptr_t initrd_end;

struct multiboot_info;

void misc_setup(const struct multiboot_info* info);

#ifdef __cplusplus
}
#endif

#endif // __MISC_H__
