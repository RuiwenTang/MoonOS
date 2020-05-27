.text
.code64
.global support_cpuid

support_cpuid:
    pushfq
    pushfq
    xorq $0x00200000, (%rsp)
    popfq
    pushfq
    pop %rax
    xorq (%rsp), %rax
    popfq

    andl $0x00200000, %eax
    ret