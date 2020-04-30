.code32
/* Multiboot macros to make a few lines more readable later */
.set MULTIBOOT_PAGE_ALIGN,        (1 << 0)
.set MULTIBOOT_MEMORY_INFO,       (1 << 1)
.set MULTIBOOT_HEADER_MAGIC,      0x1BADB002
.set MULTIBOOT_HEADER_FLAGS,      MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
.set MULTIBOOT_CHECKSUM,          -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)


// defined in linker script
.extern kernel_start
.extern kernel_end

.section .mboot
/* THIS part must be 4 byte aligned */
.align 4
/* This is GRUB multiboot header. A boot signature */
.long MULTIBOOT_HEADER_MAGIC
.long MULTIBOOT_HEADER_FLAGS
.long MULTIBOOT_CHECKSUM


// kernel stack
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:


// main entry
.section .text
.global start
.type start, @function
start:
    // setup stack
    mov $stack_top,     %esp
    call kernel_main

    cli
1:
    hlt
    jmp 1b

.size start, . - start