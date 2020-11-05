#include <moon/hal.hpp>
#include <moon/idt.hpp>
#include <moon/paging.hpp>

HAL* HAL::Instance() {
  static HAL hal{};

  return &hal;
}

void HAL::Init(multiboot_info_t* mb_info) { InitCore(mb_info); }

void HAL::InitCore(multiboot_info_t* mb_info) {
  IDT::Init();
  Paging::Instance()->InitVirtualMemory();
}