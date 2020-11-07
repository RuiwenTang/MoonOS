/*******************************************************************************
 * Project: moon-os                                                            *
 * File Created: Monday, 2nd November 2020 9:39:56 pm                          *
 * Author: tangruiwen (tangruiwen1989@gmail.com)                               *
 * Copyright - 2020                                                            *
 ******************************************************************************/
#include <multiboot/multiboot.h>
#include <moon/tty.hpp>
#include <moon/hal.hpp>
#include "kprintf.hpp"

extern "C" void main(multiboot_info_t* mb_info) { 
    
    TTY::Instance()->Init();
    HAL::Instance()->Init(mb_info);
    return; 
}