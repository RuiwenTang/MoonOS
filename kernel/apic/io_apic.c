#include <moonos/apic/io_apic.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>

#define IO_APIC_ID 0x00
#define IO_APIC_VR 0x01
#define IO_APIC_ARB 0x02
#define IO_APIC_RET_TBL(n) (0x10 + 2 * n)

static uint8_t* io_apic_base = 0;

void io_apic_set_base(uintptr_t addr) { io_apic_base = (uint8_t*)va(addr); }

static uint32_t io_apic_read(int offs) {
    uint32_t value;

    __asm__("movl (%1), %0" : "=r"(value) : "r"(io_apic_base + offs));

    return value;
}

static void io_apic_write(int offs, uint32_t value) {
    __asm__ volatile("movl %0, (%1)" : : "r"(value), "r"(io_apic_base + offs));
}

void io_apic_set_entry(uint32_t index, uint64_t data) {
    io_apic_write(IO_APIC_RET_TBL(index), (uint32_t)data);
    io_apic_write(IO_APIC_RET_TBL(index) + 1, (uint32_t)(data >> 32));
}

void io_apic_init() {
    if (io_apic_base == 0) {
        kprintf("io_apic_base is NULL\n");
        while (1)
            ;
    }

    // get number of entries supported by the IO APIC
    uint32_t x = io_apic_read(IO_APIC_VR);
    uint32_t count = ((x >> 16) & 0xff) + 1;
    // disable all entries
    for (uint32_t i = 0; i < count; i++) {
        io_apic_set_entry(i, 1 << 16);
    }
}