#include <moonos/time.h>

#include <moonos/apic/local_apic.h>
#include <moonos/interupt.h>
#include <moonos/ioport.h>
#include <moonos/kprintf.h>
#include <moonos/thread/thread.h>
#include <stdint.h>

static const uint32_t TIMER_PREIODIC = (1 << 17);
static const uint32_t TIMER_DIV128 = 10;
static const uint32_t TIMER_INIT = 262144;

// ------------------------------------------------------------------------------------------------
// I/O Ports

#define PIT_COUNTER0 0x40
#define PIT_CMD 0x43

// ------------------------------------------------------------------------------------------------
// Command Register

// BCD
#define CMD_BINARY 0x00  // Use Binary counter values
#define CMD_BCD 0x01     // Use Binary Coded Decimal counter values

// Mode
#define CMD_MODE0 0x00  // Interrupt on Terminal Count
#define CMD_MODE1 0x02  // Hardware Retriggerable One-Shot
#define CMD_MODE2 0x04  // Rate Generator
#define CMD_MODE3 0x06  // Square Wave
#define CMD_MODE4 0x08  // Software Trigerred Strobe
#define CMD_MODE5 0x0a  // Hardware Trigerred Strobe

// Read/Write
#define CMD_LATCH 0x00
#define CMD_RW_LOW 0x10   // Least Significant Byte
#define CMD_RW_HI 0x20    // Most Significant Byte
#define CMD_RW_BOTH 0x30  // Least followed by Most Significant Byte

// Counter Select
#define CMD_COUNTER0 0x00
#define CMD_COUNTER1 0x40
#define CMD_COUNTER2 0x80
#define CMD_READBACK 0xc0

// ------------------------------------------------------------------------------------------------

#define PIT_FREQUENCY 1193182

void timer_handler(void) { scheduler_tick(); }

static void local_apic_timer_setup(void) {
    const int intno = allocate_interrupt();
    register_interrupt_handler(intno, &timer_handler);

    local_apic_write(LOCAL_APIC_TIMER_DIVIDER, TIMER_DIV128);
    local_apic_write(LOCAL_APIC_TIMER_LVT, TIMER_PREIODIC | intno);
    local_apic_write(LOCAL_APIC_TIMER_INIT, TIMER_INIT);
}

#define LOCAL_APIC 1

int time_setup(void) {
#ifdef LOCAL_APIC
    local_apic_timer_setup();
    return -1;
#else
    uint32_t hz = 1000;
    uint32_t divisor = PIT_FREQUENCY / hz;

    out8(PIT_CMD, CMD_BINARY | CMD_MODE3 | CMD_RW_BOTH | CMD_COUNTER0);
    out8(PIT_COUNTER0, divisor);
    out8(PIT_COUNTER0, divisor >> 8);

    int intno = allocate_interrupt();
    register_interrupt_handler(intno, &timer_handler);

    return intno;
#endif
}