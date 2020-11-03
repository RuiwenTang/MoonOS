#include <multiboot/multiboot.h>
#include <stdio.h>

extern "C" void main(multiboot_info_t* mb_info) { 
    char buffer[100];
    ksnprintf(buffer, 100, "Hello World");
    
    return; 
}