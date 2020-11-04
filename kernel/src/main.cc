#include <multiboot/multiboot.h>
#include <moon/tty.hpp>
#include <moon/hal.hpp>
#include "kprintf.hpp"

extern "C" void main(multiboot_info_t* mb_info) { 
    
    TTY::Instance()->Init();
    HAL::Init(mb_info);
    return; 
}