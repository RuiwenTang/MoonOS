#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct IDTEntry {
  uint16_t base_low;
  uint16_t sel;
  uint8_t ist;
  uint8_t flags;
  uint16_t base_med;
  uint32_t base_high;
  uint32_t null;
} __attribute__((packed));

struct IDTPointer {
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));

typedef void (*isr_t)(void*, void*);

#ifdef __cplusplus
}
#endif

class IDT {
 public:
  IDT() = delete;
  ~IDT() = delete;

  static void Init();
  static void RegisterInterruptHandler(uint8_t interrupt, isr_t handler,
                                       void* data = nullptr);
  static void DisablePIC();
  static uint8_t ReserveUnusedInterrupt();
  static int GetErrCode();

 private:
  static void IPIHalt(void*, void*);
  static void InvalidInterruptHandler(void*, void* r);
  static void SetGate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags,
                      uint8_t ist = 0);
};

