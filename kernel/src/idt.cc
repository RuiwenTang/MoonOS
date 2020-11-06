#include <moon/idt.hpp>
#include <moon/io.hpp>
#ifdef DEBUG
#include "kprintf.hpp"
#endif

static IDTEntry gIdts[256];
static int errCode = 0;

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

extern "C" void idt_flush(IDTPointer* idt_ptr);

extern uint64_t int_vectors[];

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

void IDT::Init() {
  IDTPointer idt_ptr;
  idt_ptr.limit = sizeof(IDTEntry) * 256 - 1;
  idt_ptr.base = reinterpret_cast<uint64_t>(&gIdts);

  for (uint32_t i = 0; i < 256; i++) {
    SetGate(i, 0, 0x08, 0x8E);
  }

  for (uint32_t i = 48; i < 256; i++) {
    SetGate(i, int_vectors[i - 48], 0x08, 0x8E);
  }

  SetGate(0, (uint64_t)isr0, 0x08, 0x8E);
  SetGate(1, (uint64_t)isr1, 0x08, 0x8E);
  SetGate(2, (uint64_t)isr2, 0x08, 0x8E);
  SetGate(3, (uint64_t)isr3, 0x08, 0x8E);
  SetGate(4, (uint64_t)isr4, 0x08, 0x8E);
  SetGate(5, (uint64_t)isr5, 0x08, 0x8E);
  SetGate(6, (uint64_t)isr6, 0x08, 0x8E);
  SetGate(7, (uint64_t)isr7, 0x08, 0x8E);
  SetGate(8, (uint64_t)isr8, 0x08, 0x8E, 2);  // Double Fault
  SetGate(9, (uint64_t)isr9, 0x08, 0x8E);
  SetGate(10, (uint64_t)isr10, 0x08, 0x8E);
  SetGate(11, (uint64_t)isr11, 0x08, 0x8E);
  SetGate(12, (uint64_t)isr12, 0x08, 0x8E);
  SetGate(13, (uint64_t)isr13, 0x08, 0x8E);
  SetGate(14, (uint64_t)isr14, 0x08, 0x8E);
  SetGate(15, (uint64_t)isr15, 0x08, 0x8E);
  SetGate(16, (uint64_t)isr16, 0x08, 0x8E);
  SetGate(17, (uint64_t)isr17, 0x08, 0x8E);
  SetGate(18, (uint64_t)isr18, 0x08, 0x8E);
  SetGate(19, (uint64_t)isr19, 0x08, 0x8E);
  SetGate(20, (uint64_t)isr20, 0x08, 0x8E);
  SetGate(21, (uint64_t)isr21, 0x08, 0x8E);
  SetGate(22, (uint64_t)isr22, 0x08, 0x8E);
  SetGate(23, (uint64_t)isr23, 0x08, 0x8E);
  SetGate(24, (uint64_t)isr24, 0x08, 0x8E);
  SetGate(25, (uint64_t)isr25, 0x08, 0x8E);
  SetGate(26, (uint64_t)isr26, 0x08, 0x8E);
  SetGate(27, (uint64_t)isr27, 0x08, 0x8E);
  SetGate(28, (uint64_t)isr28, 0x08, 0x8E);
  SetGate(29, (uint64_t)isr29, 0x08, 0x8E);
  SetGate(30, (uint64_t)isr30, 0x08, 0x8E);
  SetGate(31, (uint64_t)isr31, 0x08, 0x8E);
  SetGate(0x69, (uint64_t)isr0x69, 0x08,
          0xEE /* Allow syscalls to be called from user mode*/, 0);  // Syscall

#ifdef DEBUG
  kprintf("idt_ptr = %X\n", ((uintptr_t)&gIdts) >> 32);
#endif
  idt_flush(&idt_ptr);

  IO::Write8(0x20, 0x11);
  IO::Write8(0xA0, 0x11);
  IO::Write8(0x21, 0x20);
  IO::Write8(0xA1, 0x28);
  IO::Write8(0x21, 0x04);
  IO::Write8(0xA1, 0x02);
  IO::Write8(0x21, 0x01);
  IO::Write8(0xA1, 0x01);
  IO::Write8(0x21, 0x00);
  IO::Write8(0xA1, 0x00);

  SetGate(32, (uint64_t)irq0, 0x08, 0x8E);
  SetGate(33, (uint64_t)irq1, 0x08, 0x8E);
  SetGate(34, (uint64_t)irq2, 0x08, 0x8E);
  SetGate(35, (uint64_t)irq3, 0x08, 0x8E);
  SetGate(36, (uint64_t)irq4, 0x08, 0x8E);
  SetGate(37, (uint64_t)irq5, 0x08, 0x8E);
  SetGate(38, (uint64_t)irq6, 0x08, 0x8E);
  SetGate(39, (uint64_t)irq7, 0x08, 0x8E);
  SetGate(40, (uint64_t)irq8, 0x08, 0x8E);
  SetGate(41, (uint64_t)irq9, 0x08, 0x8E);
  SetGate(42, (uint64_t)irq10, 0x08, 0x8E);
  SetGate(43, (uint64_t)irq11, 0x08, 0x8E);
  SetGate(44, (uint64_t)irq12, 0x08, 0x8E);
  SetGate(45, (uint64_t)irq13, 0x08, 0x8E);
  SetGate(46, (uint64_t)irq14, 0x08, 0x8E);
  SetGate(47, (uint64_t)irq15, 0x08, 0x8E);

  __asm__ __volatile__("sti");

  RegisterInterruptHandler(IPI_HALT, IPIHalt);
}

void IDT::RegisterInterruptHandler(uint8_t interrupt, isr_t handler,
                                   void* data) {
  gInterruptHandlers[interrupt].handler = handler;
  gInterruptHandlers[interrupt].data = data;
}

uint8_t IDT::ReserveUnusedInterrupt() {
  uint8_t interrupt = 0xFF;
  for (uint32_t i = IRQ0 + 16; i < 255 && interrupt == 0xFF; i++) {
    if (!gInterruptHandlers[i].handler) {
      interrupt = i;
    }
  }
  return interrupt;
}

void IDT::DisablePIC() {
  IO::Write8(0x20, 0x11);
  IO::Write8(0xA0, 0x11);
  IO::Write8(0x21, 0xF0);  // Remap IRQs on both PICs to 0xF0-0xF8
  IO::Write8(0xA1, 0xF0);
  IO::Write8(0x21, 0x04);
  IO::Write8(0xA1, 0x02);
  IO::Write8(0x21, 0x01);
  IO::Write8(0xA1, 0x01);

  IO::Write8(0x21, 0xFF);  // Mask all interrupts
  IO::Write8(0xA1, 0xFF);
}

int IDT::GetErrCode() { return errCode; }

extern "C" void isr_handler(int int_num, void* regs, int err_code) {
  errCode = err_code;
  if (gInterruptHandlers[int_num].handler != 0) {
    gInterruptHandlers[int_num].handler(gInterruptHandlers[int_num].data, regs);
  } else if (int_num == 0x69) {
#ifdef DEBUG
    kprintf("sys call\n");
#endif
  } else {
#ifdef DEBUG
    kprintf("isr int_num = %d\n", int_num);
#endif
  }
}

extern "C" void irq_handler(int int_num, void* regs) {}
extern "C" void ipi_handler(int int_num, void* regs) {}
