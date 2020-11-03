#include <moon/idt.hpp>

static IDTEntry gIdts[256];

struct ISRDataPair {
  isr_t handler;
  void* data;
};

ISRDataPair gInterruptHandlers[256];

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();

extern "C" void isr0x69();

extern "C" void idt_flush();

void IDT::IPIHalt(void*, void*) {
  asm("cli");
  asm("hlt");
}

void IDT::InvalidInterruptHandler(void*, void*) {}

void IDT::SetGate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags,
                  uint8_t ist) {
  gIdts[num].base_high = (base >> 32);
  gIdts[num].base_med = (base >> 16) & 0xFFFF;
  gIdts[num].base_low = base & 0xFFFF;

  gIdts[num].sel = sel;
  gIdts[num].null = 0;
  gIdts[num].ist = ist & 0x7;

  gIdts[num].flags = flags;
}

