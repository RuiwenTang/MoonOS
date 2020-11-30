/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Wednesday, 4th November 2020 2:32:35 pm                       *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include <moon/hal.hpp>
#include <moon/idt.hpp>
#include <moon/paging.hpp>

#include "balloc/balloc.hpp"

HAL* HAL::Instance() {
  static HAL hal{};

  return &hal;
}

void HAL::Init(multiboot_info_t* mb_info) { InitCore(mb_info); }

void HAL::InitCore(multiboot_info_t* mb_info) {
  IDT::Init();
  Paging::Instance()->InitVirtualMemory();
  // init bootstrap memory allocator
  Balloc::Instance()->Init(mb_info);


}