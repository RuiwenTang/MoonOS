#ifndef __IO_APIC_H__
#define __IO_APIC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void io_apic_set_base(uintptr_t addr);

void io_apic_init();
void io_apic_set_entry(uint32_t index, uint64_t data);

#ifdef __cplusplus
}
#endif

#endif  // __IO_APIC_H__