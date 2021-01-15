/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 4th November 2020 2:32:35 pm                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include <moon/acpi.hpp>
#include <moon/hal.hpp>
#include <moon/idt.hpp>
#include <moon/paging.hpp>
#include <moon/timer.hpp>

#include "balloc/balloc.hpp"
#include "kprintf.hpp"

HAL* HAL::Instance() {
  static HAL hal{};

  return &hal;
}

void HAL::Init(multiboot_info_t* mb_info) {
  InitCore(mb_info);
  InitExtra();
}

void HAL::InitCore(multiboot_info_t* mb_info) {
  IDT::Init();
  Paging::Instance()->InitVirtualMemory();
  // init bootstrap memory allocator
  // from now on we can use new and delete
  Balloc::Instance()->Init(mb_info);
  Timer::Instance()->Initialize(1000);
}

void HAL::InitExtra() { ACPI::Init(); }
