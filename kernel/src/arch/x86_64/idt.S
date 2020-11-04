
.code64

.extern isr_handler
.extern irq_handler
.extern ipi_handler
.extern LocalAPICEOI

.global idt_flush
.global int_vectors


.section .text 
.macro pushaq
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
.endm

.macro popaq
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
.endm

.macro ISR_ERROR_CODE code
.global isr\code
isr\code:
    cli
    pushq 40(%rsp)  /* ss */
    pushq 40(%rsp)  /* RSP */
    pushq 40(%rsp)  /* RFLAGS */
    pushq 40(%rsp)  /* CS */
    pushq 40(%rsp)  /* RIP */
    pushaq
    movq $\code, %rdi
    movq %rsp, %rsi
    movq 160(%rsp), %rdx
    xorq %rbp, %rbp
    call isr_handler
    popaq
    iretq
.endm

.macro ISR_NO_ERROR_CODE code
.global isr\code
isr\code:
    cli
    pushaq
    movq $\code, %rdi
    movq %rsp, %rsi
    xorq %rdx, %rdx
    xorq %rbp, %rbp
    call isr_handler
    popaq
    iretq
.endm

.altmacro
.macro IPI code
.global ipi\code
ipi\code:
    cli
    pushaq
    movq $\code, %rdi
    movq %rsp, %rsi
    xorq %rdx, %rdx
    xor %rbp, %rbp
    call ipi_handler
    popaq
    iretq
.endm

.macro IRQ code, code2
.global irq\code
irq\code:
    cli
    pushaq
    movq $\code2, %rdi
    movq %rsp, %rsi
    xorq %rdx, %rdx
    xorq %rbp, %rbp
    call irq_handler
    popaq
    iretq
.endm

idt_flush:
    lidt (%rdi)
        ret
    iretq


ISR_NO_ERROR_CODE  0
ISR_NO_ERROR_CODE  1
ISR_NO_ERROR_CODE  2
ISR_NO_ERROR_CODE  3
ISR_NO_ERROR_CODE  4
ISR_NO_ERROR_CODE  5
ISR_NO_ERROR_CODE  6
ISR_NO_ERROR_CODE  7
ISR_ERROR_CODE 8
ISR_NO_ERROR_CODE  9
ISR_ERROR_CODE 10
ISR_ERROR_CODE 11
ISR_ERROR_CODE 12
ISR_ERROR_CODE 13
ISR_ERROR_CODE 14
ISR_NO_ERROR_CODE  15
ISR_NO_ERROR_CODE  16
ISR_ERROR_CODE  17
ISR_NO_ERROR_CODE  18
ISR_NO_ERROR_CODE 19
ISR_NO_ERROR_CODE 20
ISR_NO_ERROR_CODE 21
ISR_NO_ERROR_CODE 22
ISR_NO_ERROR_CODE 23
ISR_NO_ERROR_CODE 24
ISR_NO_ERROR_CODE 25
ISR_NO_ERROR_CODE 26
ISR_NO_ERROR_CODE 27
ISR_NO_ERROR_CODE 28
ISR_NO_ERROR_CODE 29
ISR_ERROR_CODE 30
ISR_NO_ERROR_CODE 31
ISR_NO_ERROR_CODE 32
ISR_NO_ERROR_CODE 0x69  /* Syscall */

.set num, 48
.rept 256-48
    IPI %num
    .set num, num + 1
.endr

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47

.altmacro
.macro IRQNAME num
    .quad ipi\num
.endm

.section .rodata
int_vectors:
.set num, 48
.rept 256-48
     IRQNAME %num
    .set num, num + 1
.endr
