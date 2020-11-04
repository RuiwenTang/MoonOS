#include <moon/hal.hpp>
#include <moon/idt.hpp>

void HAL::Init(multiboot_info_t* mb_info) {
    InitCore(mb_info);
}

void HAL::InitCore(multiboot_info_t* mb_info) { IDT::Init(); }