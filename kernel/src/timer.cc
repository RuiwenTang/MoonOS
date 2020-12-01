/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Monday, 30th November 2020 6:14:05 pm                         *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/

#include <moon/idt.hpp>
#include <moon/io.hpp>
#include <moon/timer.hpp>
#ifdef DEBUG
#include "kprintf.hpp"
#endif

Timer* Timer::Instance() {
  static Timer gTimer{};

  return &gTimer;
}

void Timer::AddTicks() {
  fTicks++;
  if (fTicks >= fFrequency) {
    fTicks -= fFrequency;
  }
}

void Timer::Handler(void*, void*) {
#ifdef DEBUG
    kprintf("1");
#endif
  Timer::Instance()->AddTicks();
}

void Timer::Initialize(uint32_t freq) {
  Timer::Instance()->fFrequency = freq;
  IDT::RegisterInterruptHandler(IRQ0, &Timer::Handler);

  uint32_t divisor = 1193182 / freq;

  IO::Write8(0x43, 0x36);

  uint8_t l = static_cast<uint8_t>(divisor & 0xFF);
  uint8_t h = static_cast<uint8_t>((divisor >> 8) & 0xFF);

  IO::Write8(0x40, l);
  IO::Write8(0x40, h);
}