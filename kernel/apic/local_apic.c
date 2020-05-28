#include <moonos/apic/local_apic.h>
#include <moonos/interupt.h>
#include <moonos/ioport.h>
#include <moonos/kprintf.h>
#include <moonos/memory.h>

static uintptr_t LOCAL_APIC_BASE;

/**
 * Just like with video buffer, local APIC registers mapped in memory.
 * This function checks APIC BASE MSR (Model Specific Register), to find
 * physical address where local APIC registers mapped.
 **/
static uintptr_t local_apic_base(void) {
    const uint64_t APIC_BASE_MSR = 0x1b;

    uint32_t low, high;

    __asm__("rdmsr" : "=a"(low), "=d"(high) : "c"(APIC_BASE_MSR));
    return (((uintptr_t)high << 32) | low) & ~((uintptr_t)(1 << 12) - 1);
}

// ------------------------------------------------------------------------------------------------
#define PIC1_CMD 0x0020
#define PIC1_DATA 0x0021
#define PIC2_CMD 0x00a0
#define PIC2_DATA 0x00a1

#define ICW1_ICW4 0x01    // ICW4 command word: 0 = not needed, 1 = needed
#define ICW1_SINGLE 0x02  // Single mode: 0 = cascade, 1 = single
#define ICW1_ADI \
    0x04  // Call address interval: 0 = interval of 8, 1 = interval of 4
#define ICW1_LTIM 0x08  // Interrupt trigger mode: 0 = edge, 1 = level
#define ICW1_INIT 0x10  // Initialization

#define ICW4_8086 0x01        // Microprocessor mode: 0=MCS-80/85, 1=8086/8088
#define ICW4_AUTO 0x02        // Auto EOI: 0 = disabled, 1 = enabled
#define ICW4_BUF_SLAVE 0x04   // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C  // Buffered mode/master
#define ICW4_SFNM 0x10        // Special fully nested is programmed

/**
 * Even though we are going to use only local APIC for simplicity, we need to
 * disable legacy PIC to avoid problems. So the function setups PIC and than
 * masks all interrupts from PIC effectively disabling it. But since i
 * provide the code without any explanation it's might look a bit magical.
 **/
static void i8259_disable(void) {
    out8(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    out8(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

    out8(PIC1_DATA, IRQ_BASE);
    out8(PIC2_DATA, IRQ_BASE + 8);

    out8(PIC1_DATA, 4);
    out8(PIC2_DATA, 2);

    out8(PIC1_DATA, ICW4_8086);
    out8(PIC2_DATA, ICW4_8086);

    out8(PIC1_DATA, 0xff);
    out8(PIC2_DATA, 0xff);
}

uint32_t local_apic_read(int offs) {
    uint32_t value;

    __asm__ volatile("movl (%1), %0"
                     : "=r"(value)
                     : "r"(LOCAL_APIC_BASE + offs));

    return value;
}

void local_apic_write(int offs, uint32_t value) {
    __asm__ volatile("movl %0, (%1)"
                     :
                     : "r"(value), "r"(LOCAL_APIC_BASE + offs));
}

void local_apic_setup(void) {
    /**
     * Since we don't support SMP, we have only one local APIC, so only one
     * possible value. Generally every local APIC must have unique logical
     * id.
     **/
    const uint32_t LDR = (1 << 24);
    /**
     * DFR stores 'interrupt model'. There are two possible interrupts
     * models and since again we have only one local APIC, both of them
     * works for us but i'm going to use 'flat model' because it's simpler.
     **/
    const uint32_t DFR = (0xf << 28);
    /**
     * TPR (Task-Priority Register) allows to block some low-priority
     * interrupts, but since we are not going to use this functionality we
     * just set it to zero.
     **/
    const uint32_t TPR = 0;
    /**
     * To actually enable local APIC we need to set spuriuos interrupt
     * vector and "APIC Software Enable" bit in spurious interrupt register.
     **/
    const uint32_t SPURIOUS = INTNO_SPURIOUS | (1 << 8);

    i8259_disable();

    const uintptr_t local_apic_phys = local_apic_base();

    kprintf("Local APIC base address 0x%llx\n",
            (unsigned long long)local_apic_phys);

    LOCAL_APIC_BASE = (uintptr_t)va(local_apic_phys);

    local_apic_write(LOCAL_APIC_TPR, TPR);
    local_apic_write(LOCAL_APIC_LDR, LDR);
    local_apic_write(LOCAL_APIC_DFR, DFR);
    local_apic_write(LOCAL_APIC_SPURIOUS, SPURIOUS);
}

uint32_t local_apic_get_id() { return local_apic_read(LOCAL_APIC_ID) >> 24; }
