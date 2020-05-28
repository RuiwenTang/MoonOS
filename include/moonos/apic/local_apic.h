#ifndef __LOCAL_APIC_H__
#define __LOCAL_APIC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LOCAL_APIC_ID 0x0020
#define LOCAL_APIC_VER 0x0030
#define LOCAL_APIC_TPR 0x80
#define LOCAL_APIC_EOI 0xb0
#define LOCAL_APIC_DFR 0x0e0
#define LOCAL_APIC_LDR 0x0d0
#define LOCAL_APIC_SPURIOUS 0xf0
#define LOCAL_APIC_TIMER_LVT 0x320
#define LOCAL_APIC_TIMER_INIT 0x380
#define LOCAL_APIC_TIMER_COUNT 0x390
#define LOCAL_APIC_TIMER_DIVIDER 0x3e0

void local_apic_write(int reg, uint32_t value);
uint32_t local_apic_read(int reg);

void local_apic_setup(void);

uint32_t local_apic_get_id();

#ifdef __cplusplus
}
#endif

#endif  // __APIC_H__