#include <moonos/time.h>

#include <moonos/apic.h>
#include <moonos/interupt.h>
#include <moonos/kprintf.h>
#include <stdint.h>

static const uint32_t TIMER_PREIODIC = (1 << 17);
static const uint32_t TIMER_DIV128 = 10;
static const uint32_t TIMER_INIT = 262144;

static void timer_handler(void) { kprintf("."); }

static void local_apic_timer_setup(void) {
    const int intno = allocate_interrupt();
    kprintf("timer intno = %d\n", intno);
    register_interrupt_handler(intno, &timer_handler);

    local_apic_write(LOCAL_APIC_TIMER_DIVIDER, TIMER_DIV128);
    local_apic_write(LOCAL_APIC_TIMER_LVT, TIMER_PREIODIC | intno);
    local_apic_write(LOCAL_APIC_TIMER_INIT, TIMER_INIT);
}

void time_setup(void) { local_apic_timer_setup(); }