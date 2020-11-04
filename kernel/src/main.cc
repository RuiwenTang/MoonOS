#include <multiboot/multiboot.h>
#include <moon/tty.hpp>
#include <moon/idt.hpp>
#include "kprintf.hpp"

extern "C" void main(multiboot_info_t* mb_info) { 
    
    TTY::Instance()->Init();
    kprintf("Hello World aa %d", sizeof(multiboot_info_t*));
    IDT::Init();

    return; 
}