#include <multiboot/multiboot.h>
#include <stdio.h>
#include <moon/tty.hpp>

extern "C" void main(multiboot_info_t* mb_info) { 
    char buffer[100];
    size_t index = ksprintf(buffer, "Hello World %d", sizeof(multiboot_info_t*));
    buffer[index] = '\0';
    
    TTY::Instance()->Init();
    TTY::Instance()->WriteString(buffer);

    return; 
}